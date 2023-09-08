#pragma once

#include "Core.h"

WZ_DEFINE_HANDLE(WzContext)

typedef enum WzContextType {
	WZ_CONTEXT_TYPE_OPENGL = 0
} WzContextType;

typedef struct WzContextCreateInfo
{
	WzContextType contextType;
	const void* pNativeWindowHandle;
} WzContextCreateInfo;

WZ_API bool WZ_CALL wzCreateContext(WzContextCreateInfo* pContextCreateInfo, WzContext* pContext);
WZ_API void WZ_CALL wzSwapBuffers(WzContext context);
WZ_API bool WZ_CALL wzDestroyContext(WzContext context);