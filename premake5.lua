function getWorkspacePath()
    local currentDir = _SCRIPT
    while currentDir do
        local directory = path.getdirectory(currentDir)
        if os.isfile(path.join(directory, "premake5.lua")) then
            return directory
        end
        currentDir = directory
    end
    return nil
end

workspace "Bezier"
    architecture "x64"
    configurations {"Debug", "Release", "Release with Debug Information"}
    outputfold = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    topfold = getWorkspacePath()

    startproject "Bezier"

    include "Bezier/Vendor/GLFW"
    include "Bezier/Vendor/glad"

    project "Bezier"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++20"
        toolset "v143"
        staticruntime "off"
        systemversion "latest"

        targetdir (path.join(topfold, "bin", outputfold, "%{prj.name}"))
        objdir (path.join(topfold, "obj", outputfold, "%{prj.name}"))

        files
        {
            "Bezier/Src/**.cpp",
            "Bezier/Src/**.h"
        }

        includedirs
        {
            "Bezier/Src",
            "Bezier/Vendor/GLFW/include",
            "Bezier/Vendor/glad/include"
        }

        links
        {
            "GLFW",
            "Glad",
            "opengl32"
        }

        filter "configurations:Debug"
        symbols "On"         
        optimize "Off"       

        filter "configurations:Release"
            symbols "Off"
            optimize "Full"        

        filter "configurations: Release with Debug Information"
            symbols "On"
            optimize "On"

        filter {}