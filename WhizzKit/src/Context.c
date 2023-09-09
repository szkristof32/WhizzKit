#include "WhizzKit/Context.h"
#include "WhizzKit/Window.h"

#include <glad/glad.h>

#include <stdio.h>

#if defined(WZ_PLATFORM_WINDOWS)
#include <Windows.h>
#include <WinUser.h>

typedef struct WzWindow_T
{
	HWND pWindow;
} WzWindow_T;

typedef struct WzOpenGLContext_T
{
	HWND pHWND;
	HDC pHDC;
	HGLRC pHGLRC;
} WzOpenGLContext_T;

WZ_API bool WZ_CALL wzCreateOpenGLContext(WzOpenGLContextCreateInfo* pContextCreateInfo, WzOpenGLContext* pContext)
{
	*pContext = malloc(sizeof(WzOpenGLContext_T));

	WzWindow window = pContextCreateInfo->pWindowHandle;
	(*pContext)->pHWND = window->pWindow;
	(*pContext)->pHDC = GetDC(window->pWindow);

	PIXELFORMATDESCRIPTOR pfd = {
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_BITMAP |
			PFD_SUPPORT_OPENGL |
			PFD_DOUBLEBUFFER |
			PFD_TYPE_RGBA,
			24,
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			32,
			0,
			0,
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
	};
	int pf = ChoosePixelFormat((*pContext)->pHDC, &pfd);
	{
		bool result = SetPixelFormat((*pContext)->pHDC, pf, &pfd);
		if (!result) return false;
	}

	(*pContext)->pHGLRC = wglCreateContext((*pContext)->pHDC);
	{
		bool result = wglMakeCurrent((*pContext)->pHDC, (*pContext)->pHGLRC);
		if (!result) return false;
	}
	{
		bool result = gladLoadGL();
		if (!result) return false;
	}

	return true;
}

void WZ_CALL wzSwapBuffersOpenGL(WzOpenGLContext pContext)
{
	SwapBuffers(pContext->pHDC);
}

bool WZ_CALL wzDestroyOpenGLContext(WzOpenGLContext pContext)
{
	bool result = true;
	result &= ReleaseDC(pContext->pHWND, pContext->pHDC);
	result &= wglDeleteContext(pContext->pHGLRC);

	free(pContext);
	return true;
}
#endif