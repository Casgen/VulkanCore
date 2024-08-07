---@diagnostic disable: undefined-global
include("Vendor/glfw.lua")
include("Vendor/glm.lua")

project("VulkanCore")
	kind("StaticLib")
	architecture("x86_64")

	language("C++")
	cppdialect("C++17")

	local output_dir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	targetdir("../bin/" .. output_dir .. "/%{prj.name}")
	objdir("../obj/" .. output_dir .. "/%{prj.name}")

	links{ "GLFW", "GLM" }

	includedirs{
		"Vendor/glfw/include/",
		"Vendor/glm/",
		"Vendor/stb/",
		"Vendor/vma/",
		"Vendor/assimp/include/",
		"Vendor/imgui/",
		"Vendor/ZMath/",
		"Vendor/meshoptimizer",
		"Src/",
	}

	files{
		"./Src/**.cpp",
		"./Src/**.h",
		"./Src/**.hpp",
		"./Vendor/stb/**.cpp",
		"./Vendor/stb/**.h",
		"./Vendor/vma/**.h",
		"./Vendor/vma/**.cpp",
		"./Vendor/vma/**.hpp",
		"./Vendor/imgui/*.h",
		"./Vendor/imgui/*.cpp",
		"./Vendor/imgui/backends/imgui_impl_glfw.*",
		"./Vendor/imgui/backends/imgui_impl_vulkan.*",
		"./Vendor/ZMath/**.cpp",
		"./Vendor/ZMath/**.h",
		"./Vendor/meshoptimizer/src/**.cpp",
		"./Vendor/meshoptimizer/src/**.h",
		"./Vendor/meshoptimizer/src/**.hpp",
	}

	defines{
		"VMA_STATIC_VULKAN_FUNCTIONS",
	}

	filter{ "system:linux" }

		-- In case of using with VulkanSDK make sure that you have VULKAN_SDK environment variable set! (for ex. /home/username/Development/VulkanSDK/1.3.250.0/x86_64)
		-- But on linux, if Vulkan is installed correctly, VulkanSDK is not needed.
		includedirs{
			"$(VULKAN_SDK)/include/",
		}

		libdirs{
			"$(VULKAN_SDK)/lib/",
		}

		-- On linux, make sure that you have installed libvulkan-dev through your package manager!
		links{ "vulkan", "pthread", "libshaderc_shared", "assimp" }

		defines{
			"_X11",
		}

	filter{ "system:windows" }

		-- make sure that you have VULKAN_SDK environment variable set! (for ex. C:/VulkanSDK/1.3.250.0/x86_64)
		includedirs{
			"$(VULKAN_SDK)/Include",
			"$(VK_SDK_PATH)/Include",
		}

		libdirs{
			"$(VULKAN_SDK)/Lib",
			"$(VK_SDK_PATH)/Lib",
			"Vendor/assimp/lib/windows-x64",
		}

		links{ "vulkan-1", "shaderc_combined", "assimp-vc143-mtd" }

		defines{ "_WIN32" }

		buildoptions{ "/MD" }

	filter("configurations:Release")
		defines{ "NDEBUG" }
		optimize("on")

	filter("configurations:Debug")
		defines{ "DEBUG" }
		symbols("on")

	filter("options:with-vulkan")
		defines{ "VK_MESH_EXT"}

	filter("options:sanitize")
		buildoptions { "-fsanitize=address -lasan -fno-sanitize-address-use-after-scope"}
		linkoptions { "-fsanitize=address -lasan -fno-sanitize-address-use-after-scope"}

	 -- GCC and Clang
    filter { "action:gmake2", "architecture:x86_64" }
        buildoptions { "-mavx" }

    -- MSVC
    filter { "action:vs*", "architecture:x86_64" }
        buildoptions { "/arch:AVX" }

