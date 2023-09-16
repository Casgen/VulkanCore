#include "GraphicsPipelineBuilder.h"
#include "glm/fwd.hpp"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_handles.hpp"
#include "vulkan/vulkan_structs.hpp"

#include "../../../Log/Log.h"
#include <stdexcept>
#include "../../glm/glm/vec4.hpp"
#include "../../glm/glm/vec2.hpp"

namespace VkCore
{

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::BindShaderModules(const std::vector<ShaderData>& shaders)
    {
        for (const auto& item : shaders)
        {
            BindShaderModule(item);
        }

        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::BindShaderModule(const ShaderData& shaderData)
    {
        vk::PipelineShaderStageCreateInfo createInfo{};

        createInfo.setPName("main");
        createInfo.setModule(CreateShaderModule(shaderData.m_Data));
        createInfo.setStage(shaderData.m_StageFlags);

        m_ShaderStageCreateInfos.emplace_back(createInfo);

        return *this;
    }

    vk::ShaderModule GraphicsPipelineBuilder::CreateShaderModule(const std::vector<uint32_t>& data)
    {
        vk::ShaderModuleCreateInfo createInfo{};
        createInfo.setCodeSize(data.size());

        if (data.data() == nullptr || data.size() == 0)
        {
            const char* errorMsg = "Couldn't construct a shader module! A pointer to the shader data is null!";

            LOG(Vulkan, Fatal, errorMsg)
            throw std::runtime_error(errorMsg);
        }

        createInfo.setPCode(data.data());

        return m_Device.CreateShaderModule(createInfo);
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::BindVertexAttributes(
        VertexAttributeBuilder& vertexAttributeBuilder, const bool resetBuilder)
    {
        m_VertexInputBinding = vertexAttributeBuilder.GetBindingDescription();
        m_VertexInputAttributes = vertexAttributeBuilder.GetAttributeDescriptions();

        if (resetBuilder)
        {
            vertexAttributeBuilder.Reset();
        }

        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetPrimitiveAssembly(const vk::PrimitiveTopology topology,
                                                                           const bool isPrimitiveRestartEnabled)
    {
        m_VertexInputAssembly.setTopology(topology);
        m_VertexInputAssembly.setPrimitiveRestartEnable(isPrimitiveRestartEnabled);

        vk::CommandBuffer buffer;

        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::AddViewport(const glm::uvec4& dimensions)
    {
        vk::Viewport viewport{};

        viewport.x = dimensions.x;
        viewport.y = dimensions.y;

        viewport.width = dimensions.z;
        viewport.height = dimensions.w;

        viewport.maxDepth = 1.0f;
        viewport.minDepth = 0.0f;

        vk::Rect2D rect{};

        rect.offset.x = dimensions.x;
        rect.offset.y = dimensions.y;

        rect.extent.width = dimensions.z;
        rect.extent.height = dimensions.w;


        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::AddViewport(const glm::uvec2& dimensions)
    {
        vk::Viewport viewport{};

        viewport.x = 0;
        viewport.y = 0;

        viewport.width = dimensions.x;
        viewport.height = dimensions.y;

        viewport.maxDepth = 1.0f;
        viewport.minDepth = 0.0f;

        vk::Rect2D rect{};

        rect.offset.x = 0;
        rect.offset.y = 0;

        rect.extent.width = dimensions.x;
        rect.extent.height = dimensions.y;

        return *this;
    }
} // namespace VkCore
