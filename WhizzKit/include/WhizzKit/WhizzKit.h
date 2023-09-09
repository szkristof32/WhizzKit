#pragma once

// Platform detection using predefined macros
#ifdef _WIN32
	/* Windows x64/x86 */
	#ifdef _WIN64
		/* Windows x64  */
		#define WZ_PLATFORM_WINDOWS
	#else
		/* Windows x86 */
		#error "x86 Builds are not supported!"
	#endif
#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>
	/* TARGET_OS_MAC exists on all the platforms
	 * so we must check all of them (in this order)
	 * to ensure that we're running on MAC
	 * and not some other Apple platform */
	#if TARGET_IPHONE_SIMULATOR == 1
		#error "iOS simulator is not supported!"
	#elif TARGET_OS_IPHONE == 1
		#define WZ_PLATFORM_IOS
		#error "IOS is not supported!"
	#elif TARGET_OS_MAC == 1
		#define WZ_PLATFORM_MACOS
		#error "macOS is not supported!"
	#else
		#error "Unknown Apple platform!"
	#endif
/* We also have to check __ANDROID__ before __linux__
 * since android is based on the linux kernel
 * it has __linux__ defined */
#elif defined(__ANDROID__)
	#define WZ_PLATFORM_ANDROID
	#error "Android is not supported!"
#elif defined(__linux__)
	#define WZ_PLATFORM_LINUX
	#error "Linux is not supported!"
#else
	/* Unknown compiler/platform */
	#error "Unknown platform!"
#endif // End of platform detection


#if defined(WZ_PLATFORM_WINDOWS)
	#define WZ_DEBUGBREAK() __debugbreak()
	#ifdef WZ_BUILD_DLL
		#define WZ_API __declspec(dllexport)
	#else
		#define WZ_API __declspec(dllimport)
	#endif
#endif

typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

#ifndef __cplusplus
typedef uint8_t bool;

#define true 1
#define false 0
#endif

#define WZ_CALL _stdcall
#define WZ_DEFINE_HANDLE(object) typedef struct object##_T* object;

WZ_DEFINE_HANDLE(WzWindow)
WZ_DEFINE_HANDLE(WzOpenGLContext)

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

typedef struct WzOpenGLContextCreateInfo
{
	const void* pWindowHandle;
} WzOpenGLContextCreateInfo;

WZ_API bool WZ_CALL wzCreateWindow(WzWindowCreateInfo* pCreateInfo, WzWindow* pWindow);
WZ_API void WZ_CALL wzPollEvents();
WZ_API bool WZ_CALL wzWaitForEvents();
WZ_API bool WZ_CALL wzWindowShouldClose(WzWindow window);
WZ_API bool WZ_CALL wzCreateOpenGLContext(WzOpenGLContextCreateInfo* pContextCreateInfo, WzOpenGLContext* pContext);
WZ_API void WZ_CALL wzSwapBuffersOpenGL(WzOpenGLContext context);
WZ_API bool WZ_CALL wzDestroyOpenGLContext(WzOpenGLContext context);