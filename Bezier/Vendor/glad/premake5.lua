project "Glad"
    kind "StaticLib"
    language "C"
    staticruntime "off"

    targetdir (path.join(topfold, "bin", outputfold, "%{prj.name}"))
    objdir (path.join(topfold, "obj", outputfold, "%{prj.name}"))

    files
    {
        "src/glad.c"
    }

    includedirs
    {
        "include"
    }

    filter "system:windows"
        staticruntime "On"
        systemversion "latest"

    filter {}

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        optimize "Off"

    filter "configurations:Release"
        runtime "Release"
        symbols "Off"
        optimize "Full"