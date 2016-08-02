#include "D3DApplication.h"
#include "resource.h"
#include <WindowsX.h>
#include <sstream>

namespace
{
	D3DApplication *g_D3DApplication = nullptr;
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return g_D3DApplication->MsgProc(hwnd, msg, wParam, lParam);
}

/****** Public Function ***************************************/

D3DApplication::D3DApplication(HINSTANCE hInstance)
:	mMainWndCaption(L"D3D11 Application"),
	mD3DDriverType(D3D_DRIVER_TYPE_HARDWARE),
	mClientWidth(800),
	mClientHeight(600),
	mEnable4xMsaa(false),

	mhAppInstance(hInstance),
	mhMainWnd(0),
	mAppPaused(false),
	mMinimized(false),
	mMaximized(false),
	mResizing(false),
	m4xMsaaQuality(0),

	mD3DDevice(nullptr),
	mD3DImmediateContext(nullptr),
	mSwapChain(nullptr),
	mDepthStencilBuffer(nullptr),
	mRenderTargetView(nullptr),
	mDepthStencilView(nullptr)
{
	ZeroMemory(&mScreenViewport, sizeof(D3D11_VIEWPORT));
	g_D3DApplication = this;
}

D3DApplication::~D3DApplication()
{
	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mSwapChain);
	ReleaseCOM(mDepthStencilBuffer);

	if (mD3DImmediateContext)
		mD3DImmediateContext->ClearState();

	ReleaseCOM(mD3DImmediateContext);
	ReleaseCOM(mD3DDevice);
}

HINSTANCE D3DApplication::AppInstance() const
{
	return mhAppInstance;
}

HWND D3DApplication::MainWnd() const
{
	return mhMainWnd;
}

float D3DApplication::AspectRatio() const
{
	return (float)( mClientWidth ) / mClientHeight;
}

int D3DApplication::Run()
{
	MSG msg = { 0 };
	mTimer.Reset();

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			mTimer.Tick();

			if (!mAppPaused)
			{
				CalculateFrameStats();
				UpdateScence(mTimer.DeltaTime());
				DrawScence();
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return (int)msg.wParam;
}

bool D3DApplication::Init()
{
	if (FAILED(InitMainWindow()))
		return false;
	if (FAILED(InitDirect3D()))
		return false;
	return true;
}

void D3DApplication::OnResize()
{
	assert(mD3DImmediateContext);
	assert(mD3DDevice);
	assert(mSwapChain);

	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mDepthStencilBuffer);

	HR(mSwapChain->ResizeBuffers(1, mClientWidth, mClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));

#pragma region Create Render TargetView
	ID3D11Texture2D *backBuffer;
	HR(mSwapChain->GetBuffer(0, __uuidof( ID3D11Texture2D ), reinterpret_cast<void**>( &backBuffer )));
	HR(mD3DDevice->CreateRenderTargetView(backBuffer, 0, &mRenderTargetView));
	ReleaseCOM(backBuffer);
#pragma endregion

#pragma region Create Depth/Stencil Buffer and View
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width		= mClientWidth;
	depthStencilDesc.Height		= mClientHeight;
	depthStencilDesc.MipLevels	= 1;
	depthStencilDesc.ArraySize	= 1;
	depthStencilDesc.Format		= DXGI_FORMAT_D24_UNORM_S8_UINT;
	if (mEnable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}
	depthStencilDesc.Usage			= D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags		= D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags		= 0;

	HR(mD3DDevice->CreateTexture2D(&depthStencilDesc, 0, &mDepthStencilBuffer));
	HR(mD3DDevice->CreateDepthStencilView(mDepthStencilBuffer, 0, &mDepthStencilView));
#pragma endregion

	mD3DImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

#pragma region Set the Viewport
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width	 = (float)mClientHeight;
	mScreenViewport.Height	 = (float)mClientHeight;
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;
#pragma endregion

	mD3DImmediateContext->RSSetViewports(1, &mScreenViewport);
}

LRESULT D3DApplication::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{  
		case WM_ACTIVATE:
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				mAppPaused = true;
				mTimer.Stop();
			}
			else
			{
				mAppPaused = false;
				mTimer.Start();
			}
			return 0;

		case WM_SIZE:
			mClientWidth = LOWORD(lParam);
			mClientHeight = HIWORD(lParam);
			if (mD3DDevice)
			{
				if (wParam == SIZE_MINIMIZED)
				{
					mAppPaused = true;
					mMinimized = true;
					mMaximized = false;
				}
				else if (wParam == SIZE_MAXIMIZED)
				{
					mAppPaused = false;
					mMinimized = false;
					mMaximized = true;
					OnResize();
				}
				else if (wParam == SIZE_RESTORED)
				{

					if (mMinimized)
					{
						mAppPaused = false;
						mMinimized = false;
						OnResize();
					}

					else if (mMaximized)
					{
						mAppPaused = false;
						mMaximized = false;
						OnResize();
					}
					else if (mResizing)
					{
						// If user is dragging the resize bars, we do not resize 
						// the buffers here because as the user continuously 
						// drags the resize bars, a stream of WM_SIZE messages are
						// sent to the window, and it would be pointless (and slow)
						// to resize for each WM_SIZE message received from dragging
						// the resize bars.  So instead, we reset after the user is 
						// done resizing the window and releases the resize bars, which 
						// sends a WM_EXITSIZEMOVE message.
					}
					else
					{
						OnResize();
					}
				}
			}
			return 0;

			// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		case WM_ENTERSIZEMOVE:
			mAppPaused = true;
			mResizing = true;
			mTimer.Stop();
			return 0;

			// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
			// Here we reset everything based on the new window dimensions.
		case WM_EXITSIZEMOVE:
			mAppPaused = false;
			mResizing = false;
			mTimer.Start();
			OnResize();
			return 0;

			// WM_DESTROY is sent when the window is being destroyed.
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

			// The WM_MENUCHAR message is sent when a menu is active and the user presses 
			// a key that does not correspond to any mnemonic or accelerator key. 
		case WM_MENUCHAR:
			// Don't beep when we alt-enter.
			return MAKELRESULT(0, MNC_CLOSE);

			// Catch this message so to prevent the window from becoming too small.
		case WM_GETMINMAXINFO:
			( (MINMAXINFO*)lParam )->ptMinTrackSize.x = 200;
			( (MINMAXINFO*)lParam )->ptMinTrackSize.y = 200;
			return 0;

		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		case WM_MOUSEMOVE:
			OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

/****** Protected Function ***************************************/

HRESULT D3DApplication::InitMainWindow()
{
	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)MainWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= mhAppInstance;
	wcex.hIcon			= LoadIcon(mhAppInstance, (LPCTSTR)IDI_ICON_SNAKE);
	wcex.hCursor		= LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(NULL_BRUSH);
	wcex.lpszMenuName	= nullptr;
	wcex.lpszClassName	= L"D3DWndClassName";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_ICON_SNAKE);

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return E_FAIL;
	}

	RECT rc = { 0, 0, mClientWidth, mClientHeight };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	mhMainWnd = CreateWindow(
		L"D3DWndClassName",
		mMainWndCaption.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		height,
		nullptr,
		nullptr,
		mhAppInstance,
		nullptr
	);
	if (!mhMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return E_FAIL;
	}

	ShowWindow(mhMainWnd, SW_SHOW);
	UpdateWindow(mhMainWnd);

	return S_OK;
}

HRESULT D3DApplication::InitDirect3D()
{
#pragma region Create Device and Context
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(
		0,					 // default adapter
		mD3DDriverType,
		0,					 // no software device
		createDeviceFlags,
		0, 0,				 // default feature level array
		D3D11_SDK_VERSION,
		&mD3DDevice,
		&featureLevel,
		&mD3DImmediateContext
	);

	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		return E_FAIL;
	}
	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
		return E_FAIL;
	}
#pragma endregion 

#pragma region Check 4X MSAA quality support
	HR(mD3DDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality));
	assert(m4xMsaaQuality > 0);
#pragma endregion

#pragma region Creat Swap Chain
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width						= mClientWidth;
	sd.BufferDesc.Height					= mClientHeight;
	sd.BufferDesc.RefreshRate.Numerator		= 60;
	sd.BufferDesc.RefreshRate.Denominator	= 1;
	sd.BufferDesc.Format					= DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;
	if (mEnable4xMsaa)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}
	sd.BufferUsage	= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount	= 1;
	sd.OutputWindow = mhMainWnd;
	sd.Windowed		= true;
	sd.SwapEffect	= DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags		= 0;

	IDXGIDevice *dxgiDevice = nullptr;
	HR(mD3DDevice->QueryInterface(__uuidof( IDXGIDevice ), (void**)&dxgiDevice));

	IDXGIAdapter *dxgiAdapter = nullptr;
	HR(dxgiDevice->GetParent(__uuidof( IDXGIAdapter ), (void**)&dxgiAdapter));

	IDXGIFactory *dxgiFactory = nullptr;
	HR(dxgiAdapter->GetParent(__uuidof( IDXGIFactory ), (void**)&dxgiFactory));
	HR(dxgiFactory->CreateSwapChain(mD3DDevice, &sd, &mSwapChain));

	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);
#pragma endregion

	OnResize();
	return S_OK;
}

void D3DApplication::CalculateFrameStats()
{
	static int frameCount = 0;
	static float timeElapsed = 0.0f;

	frameCount++;

	if (( mTimer.TotalTime() - timeElapsed ) >= 1.0f)
	{
		float fps = (float)frameCount;
		float mspf = 1000.0f / fps;

		std::wostringstream outs;
		outs.precision(6);
		outs << mMainWndCaption << L"    "
			 << L"FPS: " << fps << L"    "
			 << L"Frame Time: " << mspf << L" (ms)";
		SetWindowText(mhMainWnd, outs.str().c_str());

		frameCount = 0;
		timeElapsed += 1.0f;
	}
}
