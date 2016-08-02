#ifndef D3DAPPLICATION_H
#define D3DAPPLICATION_H

#include "D3DUtility.h"
#include "GameTimer.h"
#include <string>


class D3DApplication
{
public:
	D3DApplication(HINSTANCE hInstance);
	virtual ~D3DApplication();

	HINSTANCE AppInstance()const;
	HWND MainWnd()const;
	float AspectRatio()const;

	int Run();

	// Framework methods.  Derived client class overrides these methods to 
	// implement specific application requirements.
	virtual bool Init();
	virtual void OnResize();
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	virtual void UpdateScence(float dt) = 0;
	virtual void DrawScence() = 0;

	virtual void OnMouseDown(WPARAM btnState, int x,int y){}
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {}
	virtual void OnMouseMove(WPARAM btnState, int x, int y) {}

protected:
	HRESULT InitMainWindow();
	HRESULT InitDirect3D();

	void CalculateFrameStats();

protected:
	HINSTANCE mhAppInstance;
	HWND mhMainWnd;
	bool mAppPaused;
	bool mMinimized;
	bool mMaximized;
	bool mResizing;
	UINT m4xMsaaQuality;

	GameTimer mTimer;

	ID3D11Device *mD3DDevice;
	ID3D11DeviceContext *mD3DImmediateContext;
	IDXGISwapChain *mSwapChain;
	ID3D11Texture2D *mDepthStencilBuffer;
	ID3D11RenderTargetView *mRenderTargetView;
	ID3D11DepthStencilView *mDepthStencilView;
	D3D11_VIEWPORT mScreenViewport;

	// Derived class should set these in derived constructor to customize starting values.
	std::wstring mMainWndCaption;
	D3D_DRIVER_TYPE mD3DDriverType;
	int mClientWidth;
	int mClientHeight;
	bool mEnable4xMsaa;
};

#endif // D3DAPPLICATION_H
