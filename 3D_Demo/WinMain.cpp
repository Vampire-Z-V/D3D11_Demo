//#include <Windows.h>
//#include "resource.h"
//
//HRESULT InitWindow(HINSTANCE, int);
//LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
//
//HINSTANCE g_hInst = nullptr;
//HWND g_hWnd = nullptr;
//
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
//{
//	UNREFERENCED_PARAMETER(hPrevInstance);
//	UNREFERENCED_PARAMETER(lpCmdLine);
//
//	if (FAILED(InitWindow(hInstance, nCmdShow)))
//		return 0;
//
//	MSG msg = { 0 };
//	while (WM_QUIT != msg.message)
//	{
//		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
//		{
//			TranslateMessage(&msg);
//			DispatchMessage(&msg);
//		}
//	}
//
//	return (int)msg.wParam;
//}
//
//HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
//{
//	WNDCLASSEX wcex;
//	wcex.cbSize = sizeof(WNDCLASSEX);
//	wcex.style = CS_HREDRAW | CS_VREDRAW;
//	wcex.lpfnWndProc = (WNDPROC)WindowProc;
//	wcex.cbClsExtra = 0;
//	wcex.cbWndExtra = 0;
//	wcex.hInstance = hInstance;
//	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_ICON_SNAKE);
//	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
//	wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
//	wcex.lpszMenuName = nullptr;
//	wcex.lpszClassName = L"TutorialWindowClass";
//	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_ICON_SNAKE);
//	if (!RegisterClassEx(&wcex))
//		return E_FAIL;
//
//	RECT rc = { 0, 0, 800, 600 };
//	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
//	g_hWnd = CreateWindow(L"TutorialWindowClass",
//						  L"Snake_Demo",
//						  WS_OVERLAPPEDWINDOW,
//						  //WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
//						  CW_USEDEFAULT,
//						  CW_USEDEFAULT,
//						  rc.right - rc.left,
//						  rc.bottom - rc.top,
//						  nullptr,
//						  nullptr,
//						  hInstance,
//						  nullptr);
//	if (!g_hWnd)
//		return E_FAIL;
//
//
//	ShowWindow(g_hWnd, SW_SHOW);
//	UpdateWindow(g_hWnd);
//
//	return S_OK;
//}
//
//
//
//LRESULT CALLBACK WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
//{
//	switch (Msg)
//	{
//		case WM_PAINT:
//			PAINTSTRUCT ps;
//			BeginPaint(hWnd, &ps);
//			EndPaint(hWnd, &ps);
//			break;
//		case WM_DESTROY:
//			PostQuitMessage(WM_QUIT);
//			break;
//		default:
//			return DefWindowProc(hWnd, Msg, wParam, lParam);
//	}
//	return 0;
//}

#include "Geosphere.h"
#include "Shape.h"
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Shape box(hInstance);
	//Geosphere box(hInstance);
	if (!box.Init())
	{
		return 0;
	}

	return box.Run();
}