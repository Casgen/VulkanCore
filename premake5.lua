---@diagnostic disable: undefined-global
project "VulkanCore"
    kind "StaticLib"

    configurations {"Debug", "Release"}

    language "C++"
    cppdialect "C++20"

    location "build"
    targetdir "bin/%{cfg.buildcfg}"
    objdir "obj/%{cfg.buildcfg}"


    links { "GLFW", "GLM" }


    includedirs {
        "Vendor/glfw/include/",
        "Vendor/glm/",
        "Vendor/stb_image",
        "Vendor/vma/",
        "/home/oem/Development/VulkanSDK/1.3.250.0/x86_64/include/"
    }

    files {
        "Src/**.cpp",
        "Src/**.h",
        "Src/**.hpp",
    }

    filter { "system:linux" }
        defines { "_X11" }

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "on"

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "on"

include "Vendor/glfw.lua"
include "Vendor/glm.lua"
