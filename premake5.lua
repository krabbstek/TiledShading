require("premake", ">=5.0.0-alpha13")

workspace "TiledShading"
    architecture "x64"
    configurations { "Debug", "Release", "Dist" }

    outputdir = "%{cfg.system}-%{cfg.architecture}-%{cfg.buildcfg}"


    project "TiledShading"
        location "TiledShading"
        kind "ConsoleApp"
        language "C++"

        targetdir ("bin/" .. outputdir .. "/%{prj.name}")
        objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

        files {
            "%{prj.name}/res/shaders/**.glsl",
            "%{prj.name}/src/**.h",
            "%{prj.name}/src/**.cpp",
            "%{prj.name}/vendor/**.h",
            "%{prj.name}/vendor/**.c",
            "%{prj.name}/vendor/**.cpp",
        }

        excludes {
            "%{prj.name}/vendor/imgui/**.h",
            "%{prj.name}/vendor/imgui/**.c",
            "%{prj.name}/vendor/imgui/**.cpp",
        }

        includedirs {
            "%{prj.name}/src",
            "%{prj.name}/vendor/glad/include",
            "%{prj.name}/vendor/imgui",
            "%{prj.name}/vendor/stb",
            "%{wks.relpath}/Dependencies/GLFW/include",
        }

        libdirs {
            "%{wks.relpath}/Dependencies/GLFW/lib-vc2015",
        }

        filter {}

        links {
            "glfw3.lib",
            "opengl32.lib",
        }

        defines {
            
        }

        filter "system:windows"
            cppdialect "C++17"
            staticruntime "On"
            systemversion "latest"

            defines {
                
            }

            prebuildcommands
            {
                ("{MKDIR} %{cfg.buildtarget.directory}")
            }
            
            postbuildcommands
            {
                --("{COPY} %{wks.relpath}/Dependencies/GLFW/lib-vc2015/glfw3.dll ../bin/" .. outputdir .. "/TiledShading")
            }

        filter "configurations:Debug"
            defines { "DEBUG" }
            buildoptions "/MDd"
            symbols "On"

        filter "configurations:Release"
            defines { "NDEBUG" }
            buildoptions "/MD"
            optimize "On"

        filter "configurations:Dist"
            defines { "NDEBUG" }
            buildoptions "/MD"
            optimize "Full"