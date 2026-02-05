workspace "WhizzKit"
	configurations { "Debug", "Release", "Dist" }
	startproject "Test"
	conformancemode "on"

	language "C++"
	cppdialect "C++23"
	staticruntime "off"
	architecture "x86_64"

	multiprocessorcompile "on"

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"NOMINMAX",
		"_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING",
	}

	filter "action:vs*"
        	linkoptions { "/ignore:4099", "/ignore:4006" }
        	disablewarnings { "4068", "4455" }
		buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

	filter "configurations:Debug"
		optimize "off"
		symbols "on"
		defines "WZ_DEBUG"

	filter "configurations:Release"
		optimize "on"
		symbols "default"
		defines "WZ_RELEASE"

	filter "configurations:Dist"
		optimize "full"
		symbols "off"
		defines "WZ_DIST"

	filter "system:windows"
		systemversion "latest"
		defines "WZ_PLATFORM_WINDOWS"

	filter "system:linux"
		defines "WZ_PLATFORM_LINUX"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "WhizzKit"
	kind "StaticLib"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"include/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"include",
	}

project "Test"
	kind "ConsoleApp"
	location "Test"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"Test/src/**.h",
		"Test/src/**.cpp"
	}

	includedirs
	{
		"include",
	}

	links
	{
		"WhizzKit"
	}

