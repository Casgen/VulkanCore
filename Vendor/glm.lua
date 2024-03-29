---@diagnostic disable: undefined-global
project "GLM"
	kind "StaticLib"
	language "C"

    architecture "x86_64"

    local output_dir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    targetdir("../../bin/" .. output_dir .. "/%{prj.name}")
    objdir("../../obj/" .. output_dir .. "/%{prj.name}")

	includedirs { "glm" }

	files
	{
		"glm/glm/**"
	}

	filter "system:linux"
		pic "On"

		systemversion "latest"
		staticruntime "On"

		defines
		{
			"_GLM_X11"
		}

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

		defines
		{
			"_GLM_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
