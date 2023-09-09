#include "WhizzKit.h"

#if defined(WZ_PLATFORM_WINDOWS)
#include <Windows.h>

typedef struct WzWindow_T
{
	HWND pWindow;
} WzWindow_T;

LRESULT CALLBACK wzWin32WindowProcess(HWND, UINT, WPARAM, LPARAM);

WZ_API bool WZ_CALL wzCreateWindow(WzWindowCreateInfo* pCreateInfo, WzWindow* pWindow)
{
	const wchar_t* CLASS_NAME = L"WzWin32Window";

	WNDCLASS wc = { 0 };
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpfnWndProc = wzWin32WindowProcess;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	int xPos = 0, yPos = 0;
	if (pCreateInfo->windowPosition == WZ_WINDOW_POSITION_CENTER_SCREEN)
	{
		int displayWidth = GetSystemMetrics(SM_CXSCREEN);
		int displayHeight = GetSystemMetrics(SM_CYSCREEN);

		xPos = (displayWidth - pCreateInfo->width) / 2;
		yPos = (displayHeight - pCreateInfo->height) / 2;
	}
	size_t length;
	mbstowcs_s(&length, NULL, 0, pCreateInfo->pTitle, 0);
	wchar_t* w_title = (wchar_t*)malloc(length * sizeof(wchar_t));
	mbstowcs(w_title, pCreateInfo->pTitle, length);

	DWORD windowStyle = WS_OVERLAPPEDWINDOW;
	if (!pCreateInfo->resizable)
		windowStyle &= ~(WS_MAXIMIZEBOX | WS_SIZEBOX);

	*pWindow = malloc(sizeof(WzWindow_T));
	(*pWindow)->pWindow = CreateWindowEx(0, CLASS_NAME, w_title, windowStyle, xPos, yPos, pCreateInfo->width, pCreateInfo->height, 0, 0, GetModuleHandle(NULL), 0);
	if (!(*pWindow)->pWindow)
	{
		free(w_title);
		free(*pWindow);
		return false;
	}

	ShowWindow((*pWindow)->pWindow, SW_SHOW);

	free(w_title);
	return true;
}

WZ_API void WZ_CALL wzPollEvents()
{
	MSG msg;
	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

WZ_API bool WZ_CALL wzWaitForEvents()
{
	return WaitMessage();
}

WZ_API bool WZ_CALL wzWindowShouldClose(WzWindow window)
{
	return IsWindow(window->pWindow);
}

LRESULT CALLBACK wzWin32WindowProcess(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		case WM_CLOSE:
		{
			DestroyWindow(hwnd);
			break;
		}
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

#endif
