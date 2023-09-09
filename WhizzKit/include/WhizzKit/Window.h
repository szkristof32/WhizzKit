#pragma once

#include "Core.h"

WZ_DEFINE_HANDLE(WzWindow)

typedef enum WzWindowPosition
{
	WZ_WINDOW_POSITION_DEFAULT = 0,
	WZ_WINDOW_POSITION_CENTER_SCREEN = 1
} WzWindowPosition;

typedef struct WzWindowCreateInfo
{
	uint32_t width;
	uint32_t height;
	const char* pTitle;
	bool resizable;
	WzWindowPosition windowPosition;
} WzWindowCreateInfo;

WZ_API bool WZ_CALL wzCreateWindow(WzWindowCreateInfo* pCreateInfo, WzWindow* pWindow);
WZ_API void WZ_CALL wzPollEvents();
WZ_API bool WZ_CALL wzWaitForEvents();
WZ_API bool WZ_CALL wzWindowShouldClose(WzWindow window);
