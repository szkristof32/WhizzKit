#pragma once

#include "Core.h"

WZ_DEFINE_HANDLE(WzOpenGLContext)

typedef struct WzOpenGLContextCreateInfo
{
	const void* pWindowHandle;
} WzOpenGLContextCreateInfo;

WZ_API bool WZ_CALL wzCreateOpenGLContext(WzOpenGLContextCreateInfo* pContextCreateInfo, WzOpenGLContext* pContext);
WZ_API void WZ_CALL wzSwapBuffersOpenGL(WzOpenGLContext context);
WZ_API bool WZ_CALL wzDestroyOpenGLContext(WzOpenGLContext context);