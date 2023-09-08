#pragma once

#include "PlatformDetection.h"

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