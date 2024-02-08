#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/types.h>

#include "ShaderData.h"
#include "ShaderLoader.h"
#include "../../FileUtils.h"
#include "shaderc/shaderc.h"
#include "shaderc/shaderc.hpp"
#include "../../Log/Log.h"
#include "shaderc/status.h"
#include "vulkan/vulkan_enums.hpp"

namespace VkCore
{
    ShaderData ShaderLoader::LoadClassicShader(const std::filesystem::path& path, const bool isOptimized)
    {
        shaderc::Compiler compiler;
        shaderc::CompileOptions compileOptions;

        if (isOptimized)
        {
            compileOptions.SetOptimizationLevel(shaderc_optimization_level_size);
        }

        return LoadClassicShader(path, compiler, compileOptions);
    }

    ShaderData ShaderLoader::LoadClassicShader(const std::filesystem::path& path, const shaderc::Compiler& compiler,
                                               const shaderc::CompileOptions& compileOptions)
    {

        std::vector<char> data = FileUtils::ReadFile(path.string());

        shaderc_shader_kind shaderKind = DetermineShaderType(path.string());

        shaderc::SpvCompilationResult result =
            compiler.CompileGlslToSpv(data.data(), shaderKind, path.stem().string().data());

        if (result.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            std::string errorMsg = "Failed to compile a shader!\nGiven file: " + path.string() +
                                   ",\nCompilation status " +
                                   ShadercCompilationStatusToString(result.GetCompilationStatus());

            LOGF(Shader, Fatal,
                 "Failed to compile a shader!\n\tGiven file: %s\n\tCompilation status: %s\n\tNumber of errors: %d "
                 "\n\tError message: %s \n\t Num of warnings: %d\n",
                 path.string().data(), ShadercCompilationStatusToString(result.GetCompilationStatus()),
                 result.GetNumErrors(), result.GetErrorMessage().data(), result.GetNumWarnings())

            throw std::runtime_error(errorMsg);
        }

        LOGF(Shader, Info, "Shader compilation was successful!\n\tGiven file: %s\n\tNum of warnings: %d\n",
             path.string().data(), result.GetNumWarnings())

        ShaderData shaderData;
        shaderData.m_Data = {result.cbegin(), result.cend()};
        shaderData.m_StageFlags = ShaderKindToClassicShaderStageFlag(shaderKind);

        return shaderData;
    }

    const char* ShaderLoader::ShadercCompilationStatusToString(shaderc_compilation_status status)
    {
        switch (status)
        {
        case shaderc_compilation_status_success:
            return "Successful";
            break;
        case shaderc_compilation_status_invalid_stage:
            return "Invalid stage (error stage deduction)";
            break;
        case shaderc_compilation_status_compilation_error:
            return "Compilator Error";
            break;
        case shaderc_compilation_status_internal_error:
            return "Unexpected internal error";
            break;
        case shaderc_compilation_status_null_result_object:
            return "Null result object";
            break;
        case shaderc_compilation_status_invalid_assembly:
            return "Invalid assembly";
            break;
        case shaderc_compilation_status_validation_error:
            return "Validation error";
            break;
        case shaderc_compilation_status_transformation_error:
            return "Transformation error";
            break;
        case shaderc_compilation_status_configuration_error:
            return "Configuration error";
            break;
        default:
            return "Not recognized";
        }
    }

    std::vector<VkCore::ShaderData> ShaderLoader::LoadClassicShaders(const std::filesystem::path& path,
                                                                     const bool isOptimized)
    {

        std::filesystem::path relativePath = std::filesystem::current_path() / path;

        if (!std::filesystem::exists(relativePath))
        {

            LOGF(Shader, Error,
                 "Failed to compile shaders! The given file system path doesn't exist! given "
                 "path: %s\n",
                 relativePath.string().data())
            return {};
        }

        if (!std::filesystem::is_directory(relativePath))
        {

            LOGF(Shader, Error,
                 "Failed to compile shaders! The given file system path is not a directory or doesn't exist! given "
                 "path: %s\n",
                 relativePath.string().data())
            return {};
        }

        shaderc::Compiler compiler;
        shaderc::CompileOptions compileOptions;

        if (isOptimized)
        {
            compileOptions.SetOptimizationLevel(shaderc_optimization_level_size);
        }

        std::vector<ShaderData> modulesMap;

        for (const auto& dirEntry : std::filesystem::directory_iterator(relativePath))
        {
            std::filesystem::path pathEntry = dirEntry.path();

            std::string extension = pathEntry.extension().string();

            if (extension == ".mesh" || extension == ".task")
            {
                LOG(Vulkan, Warning, "Found a task or mesh shader! Skipping!")
                continue;
            }

            LOGF(Shader, Info, "Found a file: %s", pathEntry.string().data())

            modulesMap.emplace_back(LoadClassicShader(pathEntry, compiler, compileOptions));
        }

        if (modulesMap.size() <= 6)
        {
            return std::move(modulesMap);
        }

        if (modulesMap.size() > 6)
        {
            LOGF(Shader, Warning,
                 "Too many shaders loaded! Are there any shaders that are duplicate?\n\tGiven path: %s",
                 path.string().data())
            return std::move(modulesMap);
        }

        if (modulesMap.size() == 0)
        {
            LOGF(Shader, Error, "No shader modules were loaded! Given path: %s", path.string().data())
        }

        return {};
    }

    std::vector<VkCore::ShaderData> ShaderLoader::LoadMeshShaders(const std::filesystem::path& path,
                                                                  const bool isOptimized)
    {
        std::filesystem::path relativePath = std::filesystem::current_path() / path;

        if (!std::filesystem::exists(relativePath))
        {

            LOGF(Shader, Error,
                 "Failed to compile shaders! The given file system path doesn't exist! given "
                 "path: %s\n",
                 relativePath.string().data())
            return {};
        }

        if (!std::filesystem::is_directory(relativePath))
        {

            LOGF(Shader, Error,
                 "Failed to compile shaders! The given file system path is not a directory or doesn't exist! given "
                 "path: %s\n",
                 relativePath.string().data())
            return {};
        }

        shaderc::Compiler compiler;
        shaderc::CompileOptions compileOptions;

        if (isOptimized)
        {
            compileOptions.SetOptimizationLevel(shaderc_optimization_level_size);
        }

        std::vector<ShaderData> modulesMap;

        for (const auto& dirEntry : std::filesystem::directory_iterator(relativePath))
        {
            std::filesystem::path pathEntry = dirEntry.path();

            std::string extension = pathEntry.extension().string();
            bool isNVExtension = pathEntry.string().find(".nv.") != -1;

            if (extension == ".mesh" || extension == ".task" || extension == ".frag")
            {
                LOGF(Shader, Info, "Found a file: %s", pathEntry.string().data())

                std::vector<char> data = FileUtils::ReadFile(pathEntry.string());

                shaderc_shader_kind shaderKind = DetermineShaderType(pathEntry.string());

                shaderc::SpvCompilationResult result =
                    compiler.CompileGlslToSpv(data.data(), shaderKind, path.stem().string().data());

                if (result.GetCompilationStatus() != shaderc_compilation_status_success)
                {
                    std::string errorMsg = "Failed to compile a shader!\nGiven file: " + path.string() +
                                           ",\nCompilation status " +
                                           ShadercCompilationStatusToString(result.GetCompilationStatus());

                    LOGF(Shader, Fatal,
                         "Failed to compile a shader!\n\tGiven file: %s\n\tCompilation status: %s\n\tNumber of errors: "
                         "%d "
                         "\n\tError message: %s \n\t Num of warnings: %d\n",
                         path.string().data(), ShadercCompilationStatusToString(result.GetCompilationStatus()),
                         result.GetNumErrors(), result.GetErrorMessage().data(), result.GetNumWarnings())

                    throw std::runtime_error(errorMsg);
                }

                LOGF(Shader, Info, "Shader compilation was successful!\n\tGiven file: %s\n\tNum of warnings: %d\n",
                     path.string().data(), result.GetNumWarnings())

                ShaderData shaderData;
                shaderData.m_Data = {result.cbegin(), result.cend()};
                shaderData.m_StageFlags = ShaderKindToMeshShaderStageFlag(shaderKind, isNVExtension);

                modulesMap.emplace_back(shaderData);

                continue;
            }

            LOG(Vulkan, Warning, "Found a task or mesh shader! Skipping!")
        }

        if (modulesMap.size() <= 3)
        {
            return std::move(modulesMap);
        }

        if (modulesMap.size() > 3)
        {
            LOGF(Shader, Warning, "Too many shaders loaded! Are there any duplicates that exist?\n\tGiven path: %s",
                 path.string().data())
            return std::move(modulesMap);
        }

        if (modulesMap.size() == 0)
        {
            LOGF(Shader, Error, "No shader modules were loaded! Given path: %s", path.string().data())
        }

        return {};
    }

    shaderc_shader_kind ShaderLoader::DetermineShaderType(const std::string& filename)
    {
        std::stringstream stringStream(filename);

        std::string tempWord;

        while (!stringStream.eof())
        {
            std::getline(stringStream, tempWord, '.');
        }

        const auto foundType = m_ShaderTypeMap.find(tempWord);

        if (foundType != m_ShaderTypeMap.end())
        {
            return foundType->second;
        }

        std::string errorMsg =
            "Couldn't determine the given shader type! the file extension couldn't be parsed!\n\tGiven path: " +
            filename;

        LOG(Shader, Error, errorMsg.data())
        throw std::runtime_error(errorMsg);
    }

    vk::ShaderStageFlagBits ShaderLoader::ShaderKindToClassicShaderStageFlag(const shaderc_shader_kind shaderKind)
    {
        switch (shaderKind)
        {
        case shaderc_vertex_shader:
            return vk::ShaderStageFlagBits::eVertex;
            break;
        case shaderc_fragment_shader:
            return vk::ShaderStageFlagBits::eFragment;
            break;
        case shaderc_compute_shader:
            return vk::ShaderStageFlagBits::eCompute;
            break;
        case shaderc_geometry_shader:
            return vk::ShaderStageFlagBits::eGeometry;
            break;
        case shaderc_tess_control_shader:
            return vk::ShaderStageFlagBits::eTessellationControl;
            break;
        case shaderc_tess_evaluation_shader:
            return vk::ShaderStageFlagBits::eTessellationEvaluation;
            break;

        case shaderc_glsl_default_vertex_shader:
            return vk::ShaderStageFlagBits::eVertex;
            break;
        case shaderc_glsl_default_fragment_shader:
            return vk::ShaderStageFlagBits::eFragment;
            break;
        case shaderc_glsl_default_compute_shader:
            return vk::ShaderStageFlagBits::eCompute;
            break;
        case shaderc_glsl_default_geometry_shader:
            return vk::ShaderStageFlagBits::eGeometry;
            break;
        case shaderc_glsl_default_tess_control_shader:
            return vk::ShaderStageFlagBits::eTessellationControl;
            break;
        case shaderc_glsl_default_tess_evaluation_shader:
            return vk::ShaderStageFlagBits::eTessellationEvaluation;
            break;

        // Ray Tracing based enums ONLY SUPPORTS KHR ONE
        case shaderc_raygen_shader:
            return vk::ShaderStageFlagBits::eRaygenKHR;
            break;
        case shaderc_anyhit_shader:
            return vk::ShaderStageFlagBits::eAnyHitKHR;
            break;
        case shaderc_closesthit_shader:
            return vk::ShaderStageFlagBits::eClosestHitKHR;
            break;
        case shaderc_miss_shader:
            return vk::ShaderStageFlagBits::eMissKHR;
            break;
        case shaderc_intersection_shader:
            return vk::ShaderStageFlagBits::eIntersectionKHR;
            break;
        case shaderc_callable_shader:
            return vk::ShaderStageFlagBits::eCallableKHR;
            break;

        case shaderc_glsl_default_raygen_shader:
            return vk::ShaderStageFlagBits::eRaygenKHR;
            break;
        case shaderc_glsl_default_anyhit_shader:
            return vk::ShaderStageFlagBits::eAnyHitKHR;
            break;
        case shaderc_glsl_default_closesthit_shader:
            return vk::ShaderStageFlagBits::eClosestHitKHR;
            break;
        case shaderc_glsl_default_miss_shader:
            return vk::ShaderStageFlagBits::eMissKHR;
            break;
        case shaderc_glsl_default_intersection_shader:
            return vk::ShaderStageFlagBits::eIntersectionKHR;
            break;
        case shaderc_glsl_default_callable_shader:
            return vk::ShaderStageFlagBits::eCallableKHR;
            break;

        case shaderc_task_shader:
            return vk::ShaderStageFlagBits::eTaskNV;
            break;
        case shaderc_mesh_shader:
            return vk::ShaderStageFlagBits::eMeshNV;
            break;

        case shaderc_glsl_default_task_shader:
            return vk::ShaderStageFlagBits::eTaskNV;
            break;
        case shaderc_glsl_default_mesh_shader:
            return vk::ShaderStageFlagBits::eMeshNV;
            break;
        default: {

            LOG(Shader, Error,
                "The given shader kind couldn't be mapped to the vulkan shader stage flag! returning "
                "vk::ShaderStageFlagBits::eAll")
            return vk::ShaderStageFlagBits::eAll;
            break;
        }
        }
    }

    vk::ShaderStageFlagBits ShaderLoader::ShaderKindToMeshShaderStageFlag(const shaderc_shader_kind shaderKind,
                                                                          bool isNvExtension)
    {

        switch (shaderKind)
        {
        case shaderc_compute_shader:
            return vk::ShaderStageFlagBits::eCompute;
        case shaderc_fragment_shader:
            return vk::ShaderStageFlagBits::eFragment;
        }

        if (isNvExtension)
        {

            switch (shaderKind)
            {
            case shaderc_task_shader:
                return vk::ShaderStageFlagBits::eTaskNV;
            case shaderc_mesh_shader:
                return vk::ShaderStageFlagBits::eMeshNV;
            }
        }

        switch (shaderKind)
        {
        case shaderc_task_shader:
            return vk::ShaderStageFlagBits::eTaskEXT;
        case shaderc_mesh_shader:
            return vk::ShaderStageFlagBits::eMeshEXT;
        }

        LOG(Shader, Error,
            "The given shader kind couldn't be mapped to the vulkan shader stage flag! returning "
            "vk::ShaderStageFlagBits::eAll")
        throw std::runtime_error("The given shader kind couldn't be mapped to the vulkan shader stage flag! returning "
                                 "vk::ShaderStageFlagBits::eAll");
        return vk::ShaderStageFlagBits::eAll;
    }

} // namespace VkCore
