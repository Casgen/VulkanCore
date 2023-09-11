---@diagnostic disable: undefined-global
include "Vendor/glfw.lua"
include "Vendor/glm.lua"

project "VulkanCore"
    kind "StaticLib"

    configurations {"Debug", "Release"}

    architecture "x86_64"

    language "C++"
    cppdialect "C++20"

    local output_dir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    targetdir("../bin/" .. output_dir .. "/%{prj.name}")
    objdir("../obj/" .. output_dir .. "/%{prj.name}")

    links { "GLFW", "GLM" }

    includedirs {
        "Vendor/glfw/include",
        "Vendor/glm",
        "Vendor/stb_image",
        "Vendor/vma/",
    }

    files {
        "Src/**.cpp",
        "Src/**.h",
        "Src/**.hpp",
        "Vendor/stb_image/**.cpp",
        "Vendor/stb_image/**.h",
        "Vendor/vma/**.h",
        "Vendor/vma/**.cpp",
        "Vendor/vma/**.hpp",
    }

    defines {
        "VMA_STATIC_VULKAN_FUNCTIONS"
    }

    filter { "system:linux" }

        -- In case of using with VulkanSDK make sure that you have VULKAN_SDK environment variable set! (for ex. /home/username/Development/VulkanSDK/1.3.250.0/x86_64)
        -- But on linux, if Vulkan is installed correctly, VulkanSDK is not needed.
        includedirs {
            "$(VULKAN_SDK)/include/"
        }

        libdirs {
            "$(VULKAN_SDK)/lib/"
        }

        -- On linux, make sure that you have installed libvulkan-dev through your package manager!
        links { "vulkan", "pthread" }


        defines {
            "_X11",
        }

    filter { "system:windows" }

        -- make sure that you have VULKAN_SDK environment variable set! (for ex. C:/VulkanSDK/1.3.250.0/x86_64)
        includedirs {
            "$(VULKAN_SDK)/Include"
        }

        libdirs {
            "$(VULKAN_SDK)/Lib"
        }

        links {
            "$(VULKAN_SDK)/Lib/vulkan-1.lib"
        }

        defines { "_WIN32" }

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "on"

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "on"


    filter { "configurations:Debug", "platforms:x64" }
        buildoptions { "/MDd" }

    filter { "configurations:Release", "platforms:Windows-x64" }
        buildoptions { "/MD" }

