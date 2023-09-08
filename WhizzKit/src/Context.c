#include "WhizzKit/Context.h"

#include <glad/glad.h>

#if defined(WZ_PLATFORM_WINDOWS)
#include <Windows.h>
#include <WinUser.h>
#endif

#include <stdio.h>

typedef struct WzContext_T
{
	const void* pContextPtr;
	WzContextType contextType;
} WzContext_T;

typedef struct WzWGLContext
{
	HWND pHWND;
	HDC pHDC;
	HGLRC pHGLRC;
} WzWGLContext;

bool WZ_CALL wzCreateOpenGLContext(WzContextCreateInfo* pContextCreateInfo, WzContext* pContext);
void WZ_CALL wzSwapBuffersWGL(WzContext pContext);
bool WZ_CALL wzDestroyOpenGLContext(WzContext pContext);

WZ_API bool WZ_CALL wzCreateContext(WzContextCreateInfo* pContextCreateInfo, WzContext* pContext)
{
	switch (pContextCreateInfo->contextType)
	{
		case WZ_CONTEXT_TYPE_OPENGL:	return wzCreateOpenGLContext(pContextCreateInfo, pContext);
	}

	fprintf(stderr, "ERROR: invalid WzContextType provided.");
	WZ_DEBUGBREAK();
	return false;
}

WZ_API void WZ_CALL wzSwapBuffers(WzContext pContext)
{
	glClear(GL_COLOR_BUFFER_BIT);
	switch (pContext->contextType)
	{
		case WZ_CONTEXT_TYPE_OPENGL:	wzSwapBuffersWGL(pContext); return;
	}

	fprintf(stderr, "ERROR: invalid WzContextType provided.");
	WZ_DEBUGBREAK();
}

WZ_API bool WZ_CALL wzDestroyContext(WzContext pContext)
{
	switch (pContext->contextType)
	{
		case WZ_CONTEXT_TYPE_OPENGL:	return wzDestroyOpenGLContext(pContext);
	}

	fprintf(stderr, "ERROR: invalid WzContextType provided.");
	WZ_DEBUGBREAK();
	return false;
}

bool WZ_CALL wzCreateOpenGLContext(WzContextCreateInfo* pContextCreateInfo, WzContext* pContext)
{
	*pContext = malloc(sizeof(WzContext_T));
	(*pContext)->contextType = WZ_CONTEXT_TYPE_OPENGL;

#if defined(WZ_PLATFORM_WINDOWS)
	(*pContext)->pContextPtr = malloc(sizeof(WzWGLContext));
	((WzWGLContext*)(*pContext)->pContextPtr)->pHWND = pContextCreateInfo->pNativeWindowHandle;
	((WzWGLContext*)(*pContext)->pContextPtr)->pHDC = GetDC((HWND)pContextCreateInfo->pNativeWindowHandle);

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
	int pf = ChoosePixelFormat(((WzWGLContext*)(*pContext)->pContextPtr)->pHDC, &pfd);
	{
		bool result = SetPixelFormat(((WzWGLContext*)(*pContext)->pContextPtr)->pHDC, pf, &pfd);
		if (!result) return false;
	}

	((WzWGLContext*)(*pContext)->pContextPtr)->pHGLRC = wglCreateContext(((WzWGLContext*)(*pContext)->pContextPtr)->pHDC);
	{
		bool result = wglMakeCurrent(((WzWGLContext*)(*pContext)->pContextPtr)->pHDC, ((WzWGLContext*)(*pContext)->pContextPtr)->pHGLRC);
		if (!result) return false;
	}
	{
		bool result = gladLoadGL();
		if (!result) return false;
	}
#endif

	return true;
}

void WZ_CALL wzSwapBuffersWGL(WzContext pContext)
{
	SwapBuffers(((WzWGLContext*)pContext->pContextPtr)->pHDC);
}

bool WZ_CALL wzDestroyOpenGLContext(WzContext pContext)
{
	bool result = true;
	result &= ReleaseDC(((WzWGLContext*)pContext->pContextPtr)->pHWND, ((WzWGLContext*)pContext->pContextPtr)->pHDC);
	result &= wglDeleteContext(((WzWGLContext*)pContext->pContextPtr)->pHGLRC);

	free(pContext);
	return true;
}