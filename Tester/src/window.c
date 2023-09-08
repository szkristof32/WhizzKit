#include "window.h"

#include <stdio.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int g_WindowShouldClose = 0; // this is just a hacky way of storing this boolean as I don't expect to have more than one window

void CreateNewWindow(uint32_t width, uint32_t height, const wchar_t* title, HWND* window)
{
	*window = malloc(sizeof(HWND));

	const wchar_t* WINDOW_CLASS = L"Window";

	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = WINDOW_CLASS;

	RegisterClass(&wc);

	*window = CreateWindowEx(0, WINDOW_CLASS, title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, GetModuleHandle(NULL), NULL);
	if (*window == NULL)
	{
		printf("ERROR: window handle is null.\n");
		__debugbreak();
	}

	ShowWindow(*window, SW_SHOW);
}

void PollWindowEvents(HWND window)
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

int WindowShouldClose(HWND window)
{
	return g_WindowShouldClose;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		g_WindowShouldClose = 1;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}