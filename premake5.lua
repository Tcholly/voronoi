workspace "voronoi_diagram"
    language "C"
    
    architecture "x86_64"
    configurations { "Debug", "Release" }

    warnings "Extra"

    filter { "configurations:Debug" }
        defines { "_DEBUG" }
        symbols "On"

    filter { "configurations:Release" }
        optimize "On"

    filter { }

    targetdir ("bin/%{prj.name}/%{cfg.longname}")
    objdir ("obj/%{prj.name}/%{cfg.longname}")

project "voronoi_diagram"
    kind "ConsoleApp"
    files "src/**"

    includedirs {
		"src",
		"Dependencies/raylib/include"
    }

	libdirs {
		"Dependencies/raylib/lib"
	}

    links { "raylib", "m" }
