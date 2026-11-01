-- Keep the legacy solution in sync using Premake's native .sln generator.
-- Only solution serialization uses the legacy format; projects still target VS 2026.
if _ACTION == "vs2026" then
    local action = premake.action.get("vs2026")
    local legacyAction = premake.action.get("vs2022")
    local generateWorkspace = action.onWorkspace

    action.onWorkspace = function(wks)
        generateWorkspace(wks)
        local solutionVersion = action.vstudio.solutionVersion
        action.vstudio.solutionVersion = legacyAction.vstudio.solutionVersion
        local success, message = pcall(legacyAction.onWorkspace, wks)
        action.vstudio.solutionVersion = solutionVersion
        if not success then error(message) end
    end
end

workspace "Hazel"
    architecture "x64"
    startproject "Sandbox"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Hazel/vendor/GLFW/include"
IncludeDir["Glad"] = "Hazel/vendor/Glad/include"

include "Hazel/vendor/GLFW"
include "Hazel/vendor/Glad"

project "Hazel"
    location "Hazel"
    kind "SharedLib"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "hzpch.h"
    pchsource "Hazel/src/hzpch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}"
    }

    links
    {
        "GLFW",
        "Glad",
        "opengl32.lib"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "Off"
        systemversion "latest"
        buildoptions { "/utf-8" }

        defines
        {
            "HZ_PLATFORM_WINDOWS",
            "HZ_BUILD_DLL",
            "GLFW_INCLUDE_NONE"
        }

        postbuildcommands
        {
            ('{MKDIR} "../bin/' .. outputdir .. '/Sandbox"'),
            ('{COPY} "%{cfg.buildtarget.relpath}" "../bin/' .. outputdir .. '/Sandbox"')
        }

    filter "configurations:Debug"
        defines { "HZ_DEBUG", "HZ_ENABLE_ASSERTS" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines "HZ_RELEASE"
        runtime "Release"
        symbols "On"
        optimize "On"

    filter "configurations:Dist"
        defines "HZ_DIST"
        runtime "Release"
        symbols "Off"
        optimize "On"


project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "Hazel/vendor/spdlog/include",
        "Hazel/src"
    }

    links
    {
        "Hazel"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "Off"
        systemversion "latest"
        buildoptions { "/utf-8" }

        defines
        {
            "HZ_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines { "HZ_DEBUG", "HZ_ENABLE_ASSERTS" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines "HZ_RELEASE"
        runtime "Release"
        symbols "On"
        optimize "On"

    filter "configurations:Dist"
        defines "HZ_DIST"
        runtime "Release"
        symbols "Off"
        optimize "On"
