require "premake5/actions"

workspace "AA_Playground_Workspace"
	configurations { "Release", "Debug" }
	platforms {	"Windows_x64" }
	characterset "Unicode"
	language "C++"
	rtti "Off"
	targetsuffix "_%{cfg.platform}"
	warnings "Default"
	flags { "FatalWarnings" }
	filter "configurations:*"
		-- TODO: does this need to be in a configurations filter? Seems a little strange
		if (_ACTION == "vs2015") then
			buildoptions{"/std:c++latest"}
		end
	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"
		libdirs { "dependencies/glfw/build/src/Debug" }
	filter "configurations:Release"
		defines { "NDEBUG", "RELEASE" }
		optimize "Full"
		libdirs { "dependencies/glfw/build/src/Release" }
	filter "platforms:*x64"
		architecture "x64"
	filter "platforms:Windows_*"
		defines {"OS_WINDOWS"}
		debugenvs {
			-- Start PATH
			--"PATH="..
			--"%{wks.location}/dependencies/".. xyz .."/bin;"..
			--"%PATH%"
			-- End PATH
		}

project "AA_Playground"
	kind "ConsoleApp"
	targetdir "bin/%{cfg.buildcfg}_%{cfg.platform}"
	objdir "obj/%{cfg.buildcfg}_%{cfg.platform}"
	debugdir "%{wks.location}/src"
	links {
		"glfw3.lib",
		"opengl32.lib"
	}
	files {
		"src/**.cpp",
		"src/**.c",
		"src/**.glsl",
		"include/**.hpp",
		"include/**.h",
	}
	includedirs {
		"include",
		"dependencies/glfw/include",
		"dependencies/tinyobjloader/include",
		"dependencies/glm"
	}
