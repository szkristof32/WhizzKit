project "WhizzKit"
	kind "SharedLib"
	language "C"
	staticruntime "off"
	
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"src/**.c",
		"src/**.h",
		"include/**.h"
	}
	
	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}
	
	includedirs
	{
		"src",
		"include",
		"include/WhizzKit",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.VulkanSDK}"
	}
	
	libdirs
	{
		"%{LibraryDir.VulkanSDK}"
	}
	
	links
	{
		"Glad"
	}
	
	postbuildcommands
	{
		("mkdir ..\\bin\\" .. outputdir .. "\\Tester"),
		("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Tester")
	}
	
	filter "system:windows"
		systemversion "latest"
		
		defines
		{
			"WZ_BUILD_DLL"
		}
		
		links
		{
			"opengl32.lib",
			"vulkan-1.lib"
		}
	
	filter "configurations:Debug"
		defines "WZ_DEBUG"
		runtime "Debug"
		symbols "on"
		
		links
		{
			"%{Library.ShaderC_Debug}",
			"%{Library.SPIRV_Cross_Debug}",
			"%{Library.SPIRV_Cross_GLSL_Debug}",
			"%{Library.SPIRV_Tools_Debug}"
		}
		
	filter "configurations:Release"
		defines "WZ_RELEASE"
		runtime "Release"
		optimize "on"
		
		links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}
		
	filter "configurations:Dist"
		defines "WZ_DIST"
		runtime "Release"
		optimize "on"
		
		links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}