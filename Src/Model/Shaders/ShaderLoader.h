#pragma once

#include <filesystem>
#include <map>

#include "vulkan/vulkan.hpp"
#include <shaderc/shaderc.hpp>
#include "ShaderData.h"
#include "vulkan/vulkan_enums.hpp"

namespace VkCore
{
    class ShaderLoader
    {

      public:
        ShaderLoader();

        /**
         * @brief Reads all the types of shaders and compiles them into SPIR-V binary in order to use them in the
         * Vulkan classic programmable graphics pipeline
         * @param path - A path to the directory with all its shaders. All shaders should be contained in one
         * directory
         * (/res/shaders/triangle_shader.frag,/res/shaders/triangle_shader.vert, ...) and you should only pass the name
         * of the directory with the shaders inside! for ex. /res/shaders/
         * @param isOptimized - Apply optimization techniques to SPIR-V. By default it's true (Could result in difficult
         * debugability)
         * @return vector of ShaderData objects
         */
        static std::vector<VkCore::ShaderData> LoadClassicShaders(const std::filesystem::path& path,
                                                                  const bool isOptimized = true);

        static std::vector<VkCore::ShaderData> LoadMeshShaders(const std::filesystem::path& path,
                                                               const bool isOptimized = true);

        static const char* ShadercCompilationStatusToString(shaderc_compilation_status status);

        /**
         * @brief Reads a shader and compiles it into SPIR-V binary in order to use it in the
         * Vulkan graphics pipeline. Ideally use this function to one time compile a shader. Under the hood a compiler
         * context object and compile options object is being created here.
         * @param sourceFilename - A path to the particular shader.
         * @param isOptimized - Apply optimization techniques to SPIR-V. By default it's true (Could result in difficult
         * debugability)
         * @return a pointer to the compiled shader module.
         */
        static VkCore::ShaderData LoadClassicShader(const std::filesystem::path& path, const bool isOptimized = true);

        static VkCore::ShaderData LoadMeshShader(const std::filesystem::path& path, const bool isOptimized = true);

        /**
         * @brief reads the given filename, obtains its file extension and deteremines the shader kind (vertex,
         * fragment, geometry etc.)
         */
        static shaderc_shader_kind DetermineShaderType(const std::string& filename);

        /**
         * @brief converts the shaderc_shader_kind enum to vk::ShaderStageFlags for usage in the graphics pipeline.
         * UPDATING: Watch out for new types of shaders!, if any of them are added by vulkan or shaderc, it has to be
         * implemented further to recognize the new ones!
         * @param shaderKind - A shaderc shader kind enum to deduce the stage flag
         */
        static vk::ShaderStageFlagBits ShaderKindToClassicShaderStageFlag(const shaderc_shader_kind shaderKind);

        /**
         * @brief converts the shaderc_shader_kind enum to vk::ShaderStageFlags for usage in the graphics pipeline.
         * UPDATING: Watch out for new types of shaders!, if any of them are added by vulkan or shaderc, it has to be
         * implemented further to recognize the new ones!
         * @param shaderKind - A shaderc shader kind enum to deduce the stage flag
         * @param isNvExtension - Determines whether to return a ray tracing
         */
        static vk::ShaderStageFlagBits ShaderKindToMeshShaderStageFlag(const shaderc_shader_kind shaderKind);

      private:
        /**
         * @brief Reads a shader and compiles it into SPIR-V binary in order to use it in the
         * Vulkan graphics pipeline. The function does not read the directory recursively!
         * @param sourceFilename - A path to the particular shader.
         * @param shaderKind - What type of shader should be the file compiled as (Vertex, fragment, geometry, etc...)
         * @param compiler - An optional reference to the compiler context object
         * @param compileOptions - An Optional reference to the compile options object;
         * @param isOptimized - Apply optimization techniques to SPIR-V. By default it's true (Could result in difficult
         * debugability)
         * @return a pointer to the compiled shader module.
         */
        static VkCore::ShaderData LoadClassicShader(const std::filesystem::path& path,
                                                    const shaderc::Compiler& compiler,
                                                    const shaderc::CompileOptions& compileOptions);

        inline static std::map<std::string, shaderc_shader_kind> m_ShaderTypeMap = {
            {"vert", shaderc_vertex_shader},
            {"frag", shaderc_fragment_shader},
            {"geom", shaderc_geometry_shader},
            {"tesc", shaderc_tess_evaluation_shader},
            {"tese", shaderc_tess_evaluation_shader},
            {"comp", shaderc_compute_shader},
#ifndef VK_MESH_EXT
            {"nv.mesh", shaderc_mesh_shader},
            {"nv.task", shaderc_task_shader}
#else
            {"mesh", shaderc_mesh_shader},
            {"task", shaderc_task_shader}
#endif
        };
    };

} // namespace VkCore
