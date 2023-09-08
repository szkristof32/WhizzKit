workspace "GraphicsLibrary"
	startproject "Tester"
	architecture "x86_64"
		
	flags
	{
		"MultiProcessorCompile"
	}

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "Dependencies.lua"

group "Dependencies"
include "WhizzKit/vendor/Glad"
group ""

include "WhizzKit"
include "Tester"