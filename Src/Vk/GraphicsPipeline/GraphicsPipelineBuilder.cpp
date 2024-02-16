#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include "glm/vec4.hpp"
#include "glm/vec2.hpp"

#include "GraphicsPipelineBuilder.h"
#include "glm/fwd.hpp"
#include "vulkan/vulkan.hpp"
#include "../../Log/Log.h"
#include "../Utils.h"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_enums.hpp"

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
        createInfo.setCodeSize(data.size() * 4);

        if (data.data() == nullptr || data.size() == 0)
        {
            const char* errorMsg = "Couldn't construct a shader module! A pointer to the shader data is null!";

            LOG(Vulkan, Fatal, errorMsg)
            throw std::runtime_error(errorMsg);
        }

        createInfo.setPCode(data.data());

        TRY_CATCH_BEGIN()

        return m_Device.CreateShaderModule(createInfo);

        TRY_CATCH_END()
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

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::AddViewport(const glm::uvec4& viewportDimensions,
                                                                  const glm::uvec4& scissorsDimensions)
    {
        vk::Viewport viewport{};

        viewport.x = viewportDimensions.x;
        viewport.y = viewportDimensions.y;

        viewport.width = viewportDimensions.z;
        viewport.height = viewportDimensions.w;

        viewport.maxDepth = 1.0f;
        viewport.minDepth = 0.0f;

        vk::Rect2D rect{};

        rect.offset.x = scissorsDimensions.x;
        rect.offset.y = scissorsDimensions.y;

        rect.extent.width = scissorsDimensions.z;
        rect.extent.height = scissorsDimensions.w;

        m_Viewports.emplace_back(viewport);
        m_Scissors.emplace_back(rect);

        return *this;
    }

    GraphicsPipelineBuilder& AddViewport(const glm::uvec2& viewportDimensions, const glm::uvec2& scissorsDimensions)
    {
        glm::uvec4 scissorsDim = {0, 0, scissorsDimensions.x, scissorsDimensions.y};
        glm::uvec4 viewportDim = {0, 0, viewportDimensions.x, viewportDimensions.y};

        return AddViewport(viewportDim, scissorsDim);
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::AddViewport(const glm::uvec4& viewportDimensions)
    {
        return AddViewport(viewportDimensions, viewportDimensions);
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::AddViewport(const glm::uvec2& viewportDimensions)
    {
        glm::uvec4 dimensions = {0, 0, viewportDimensions.x, viewportDimensions.y};
        return AddViewport(dimensions, dimensions);
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::AddViewport(const vk::Viewport& viewport)
    {
        vk::Rect2D rect{};

        rect.offset.x = static_cast<int32_t>(viewport.x);
        rect.offset.y = static_cast<int32_t>(viewport.y);

        rect.extent.width = static_cast<uint32_t>(viewport.width);
        rect.extent.height = static_cast<uint32_t>(viewport.height);

        m_Viewports.emplace_back(viewport);
        m_Scissors.emplace_back(rect);

        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::AddViewport(const vk::Viewport& viewport,
                                                                  const vk::Rect2D& scissor)
    {
        m_Viewports.emplace_back(viewport);
        m_Scissors.emplace_back(scissor);

        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::AddDynamicState(const vk::DynamicState dynamicState)
    {
        m_DynamicStates.emplace_back(dynamicState);
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::EnableDepthClamping(const bool isEnabled)
    {
        m_RasterizationInfo.setDepthClampEnable(isEnabled);

        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetPolygonMode(const vk::PolygonMode polygonMode)
    {
        m_RasterizationInfo.setPolygonMode(polygonMode);
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::EnabledDiscard(const bool isEnabled)
    {
        m_RasterizationInfo.setRasterizerDiscardEnable(isEnabled);
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::FrontFaceDirection(const vk::FrontFace frontFace)
    {
        m_RasterizationInfo.setFrontFace(frontFace);
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetCullMode(const vk::CullModeFlags cullMode)
    {
        m_RasterizationInfo.setCullMode(cullMode);
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::EnableDepthBias(const bool isEnabled)
    {
        m_RasterizationInfo.setDepthBiasEnable(true);
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetDepthBiasParams(const float constantFactor, const float clamp,
                                                                         const float slopeFactor)
    {
        if (!m_RasterizationInfo.depthBiasEnable)
        {
            LOG(Vulkan, Warning,
                "Called the function for setting depth bias parameters while Depth bias is not enabled! Implicitly "
                "turning on Depth bias!")

            m_RasterizationInfo.setDepthBiasEnable(true);
        }

        m_RasterizationInfo.setDepthBiasConstantFactor(constantFactor)
            .setDepthBiasSlopeFactor(slopeFactor)
            .setDepthBiasClamp(clamp);

        return *this;
    }
    GraphicsPipelineBuilder& GraphicsPipelineBuilder::AddBlendAttachment(
        const vk::PipelineColorBlendAttachmentState blendAttachment)
    {
        m_ColorBlendAttachmentStates.emplace_back(blendAttachment);

        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::AddDisabledBlendAttachment()
    {
        vk::PipelineColorBlendAttachmentState state{};

        state.setBlendEnable(false);
        state.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

        m_ColorBlendAttachmentStates.emplace_back(state);

        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetLineWidth(const float lineWidth)
    {
        m_RasterizationInfo.setLineWidth(lineWidth);

        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::AddDescriptorLayout(
        const std::vector<vk::DescriptorSetLayout> layouts)
    {
        for (const auto& layout : layouts)
        {
            m_DescriptorSetLayouts.emplace_back(layout);
        }

        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::AddDescriptorLayout(const vk::DescriptorSetLayout layout)
    {
        m_DescriptorSetLayouts.emplace_back(layout);
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetDescriptorLayout(
        const std::vector<vk::DescriptorSetLayout>& layouts)
    {
        m_DescriptorSetLayouts.clear();
        m_DescriptorSetLayouts = layouts;

        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::AddPushConstantRange(const vk::PushConstantRange& range)
    {
        m_PushConstantRanges.emplace_back(range);
        return *this;
    }

    GraphicsPipelineBuilder& GraphicsPipelineBuilder::BindRenderPass(const vk::RenderPass& renderPass)
    {
        m_PipelineCreateInfo.renderPass = renderPass;
        return *this;
    }

    vk::Pipeline GraphicsPipelineBuilder::Build()
    {
        vk::PipelineLayout pipelineLayout;
        return Build(pipelineLayout);
    }

    vk::Pipeline GraphicsPipelineBuilder::Build(vk::PipelineLayout& pipelineLayout)
    {
        ValidateShaderInfo();
        ValidateVertexInfo();
        ValidateViewportInfo();

        m_PipelineCreateInfo.setStages(m_ShaderStageCreateInfos);

        vk::PipelineVertexInputStateCreateInfo vertexInputState{};

        vertexInputState.setVertexBindingDescriptions(m_VertexInputBinding);
        vertexInputState.setVertexAttributeDescriptions(m_VertexInputAttributes);

        vk::PipelineViewportStateCreateInfo viewportStateCreateInfo{};
        viewportStateCreateInfo.setScissors(m_Scissors);
        viewportStateCreateInfo.setViewports(m_Viewports);

        m_BlendStateCreateInfo.setAttachments(m_ColorBlendAttachmentStates);

        vk::PipelineLayoutCreateInfo layoutCreateInfo;

        layoutCreateInfo.setSetLayouts(m_DescriptorSetLayouts).setPushConstantRanges({});

        TRY_CATCH_BEGIN()

        pipelineLayout = m_Device.CreatePipelineLayout(layoutCreateInfo);

        TRY_CATCH_END()

        if (!m_PipelineCreateInfo.renderPass)
        {
            const char* errorMsg = "Failed to create a graphics pipeline! The Render pass was not set! (nullptr)";
            LOG(Vulkan, Fatal, errorMsg)
            throw std::runtime_error(errorMsg);
        }

        vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
        dynamicStateCreateInfo.setDynamicStates(m_DynamicStates);

        m_PipelineCreateInfo.setSubpass(0)
            .setLayout(pipelineLayout)
            .setPDynamicState(&dynamicStateCreateInfo)
            .setPColorBlendState(&m_BlendStateCreateInfo)
            .setSubpass(0)
            .setPViewportState(&viewportStateCreateInfo)
            .setPStages(m_ShaderStageCreateInfos.data())
            .setPMultisampleState(&m_MultisampleCreateInfo)
            .setStageCount(m_ShaderStageCreateInfos.size())
            .setPDepthStencilState(nullptr)
            .setPRasterizationState(&m_RasterizationInfo);

        if (m_VertexInputAttributes.size() == 0)
            m_PipelineCreateInfo.setPVertexInputState(nullptr);
        else
            m_PipelineCreateInfo.setPVertexInputState(&vertexInputState);

        m_PipelineCreateInfo
            .setPInputAssemblyState(&m_VertexInputAssembly)
            // These member variables are for creating a derivative of this pipeline. Won't be used here.
            .setBasePipelineIndex(-1)
            .setBasePipelineHandle(VK_NULL_HANDLE);

        TRY_CATCH_BEGIN()

        auto layout = m_Device.CreateGraphicsPipelines({m_PipelineCreateInfo});

        VkCore::Utils::CheckVkResult(layout.result);

        LOG(Vulkan, Info, "Graphics Pipeline was successfully created!")

        for (const vk::PipelineShaderStageCreateInfo& info : m_ShaderStageCreateInfos)
        {
            m_Device.DestroyShaderModule(info.module);
            LOG(Vulkan, Verbose, "vk::ShaderModule destroyed.")
        }

        return layout.value[0];

        TRY_CATCH_END()
    }

    void GraphicsPipelineBuilder::ValidateShaderInfo()
    {

        if (m_ShaderStageCreateInfos.size() < 2)
        {

            if (m_IsMeshShading)
            {

                const char* errorMsg =
                    "Not enough shaders provided! There has to be atleast a fragment and a mesh shader!";
                LOG(Vulkan, Fatal, errorMsg)
                throw std::runtime_error(errorMsg);
            }
            else
            {

                const char* errorMsg =
                    "Not enough shaders provided! There has to be atleast a fragment and a vertex shader!";
                LOG(Vulkan, Fatal, errorMsg)
                throw std::runtime_error(errorMsg);
            }
        }

        bool isFragmentStagePresent = false;
        uint32_t stageBitfield = 0;

        if (m_IsMeshShading)
        {

            bool isMeshStagePresent = false;

            for (const auto& info : m_ShaderStageCreateInfos)
            {

                uint32_t stage = static_cast<uint32_t>(info.stage);

                isMeshStagePresent |= stage & VK_SHADER_STAGE_MESH_BIT_EXT;
                isFragmentStagePresent |= stage & VK_SHADER_STAGE_FRAGMENT_BIT;

                if (stageBitfield & stage)
                {
                    const char* errorMsg = "2 or more shaders of the same type were provided!";
                    LOG(Vulkan, Fatal, errorMsg)
                    throw std::runtime_error(errorMsg);
                }

                stageBitfield |= stage;
            }

            if (!isFragmentStagePresent)
            {
                const char* errorMsg = "Failed to create the pipeline! Fragment shader not present!";
                LOG(Vulkan, Fatal, errorMsg)
                throw std::runtime_error(errorMsg);
            }

            if (!isMeshStagePresent)
            {
                const char* errorMsg = "Failed to create the pipeline! Mesh shader not present!";
                LOG(Vulkan, Fatal, errorMsg)
                throw std::runtime_error(errorMsg);
            }

            return;
        }

        bool isVertexStagePresent = false;

        for (const auto& info : m_ShaderStageCreateInfos)
        {

            uint32_t stage = static_cast<uint32_t>(info.stage);

            isVertexStagePresent |= stage & VK_SHADER_STAGE_VERTEX_BIT;
            isFragmentStagePresent |= stage & VK_SHADER_STAGE_FRAGMENT_BIT;

            if (stageBitfield & stage)
            {
                const char* errorMsg = "2 or more shaders of the same type were provided!";
                LOG(Vulkan, Fatal, errorMsg)
                throw std::runtime_error(errorMsg);
            }

            stageBitfield |= stage;
        }

        if (!isFragmentStagePresent)
        {
            const char* errorMsg = "Failed to create the pipeline! Fragment shader not present!";
            LOG(Vulkan, Fatal, errorMsg)
            throw std::runtime_error(errorMsg);
        }

        if (!isVertexStagePresent)
        {
            const char* errorMsg = "Failed to create the pipeline! Vertex shader not present!";
            LOG(Vulkan, Fatal, errorMsg)
            throw std::runtime_error(errorMsg);
        }
    }

    bool GraphicsPipelineBuilder::ValidateVertexInfo()
    {
        if (m_VertexInputAttributes.size() == 0)
        {
            LOG(Vulkan, Warning,
                "No input attributes defined! Setting input binding descriptions and attribute description as blank!")
            return false;
        }

        uint32_t actualBinding = m_VertexInputBinding.binding;
        uint32_t expectedLocationCount = 0, actualLocationCount = 0;

        for (int i = 0; i < m_VertexInputAttributes.size(); i++)
        {
            if (actualBinding != m_VertexInputAttributes[i].binding)
            {
                LOGF(Vulkan, Warning,
                     "Vulkan input attribute with location %d and binding %d doesn't correspond with the binding in "
                     "the vertex input binding %d!",
                     m_VertexInputAttributes[i].location, m_VertexInputAttributes[i].binding, actualBinding)
                return false;
            }

            expectedLocationCount += i;
            actualLocationCount += m_VertexInputAttributes[i].location;
        }

        if (expectedLocationCount != actualLocationCount)
        {
            LOG(Vulkan, Warning,
                "Locations are not incrementing correctly! Some index/indices are missing or set incorrectly!")
            return true;
        }

        return true;
    }

    void GraphicsPipelineBuilder::ValidateViewportInfo()
    {
        if (m_Viewports.size() != m_Scissors.size())
        {
            LOG(Vulkan, Error,
                "The number of viewports doesn't equal the number of scissors! Either add/remove a viewport/scissor!");
        }
    }

    void GraphicsPipelineBuilder::Init()
    {
        m_RasterizationInfo.setLineWidth(1.f);

        // TODO: Test them if they are actually set right. There might be possiblity that the polygons won't draw!
        m_RasterizationInfo.setFrontFace(vk::FrontFace::eClockwise);
        m_RasterizationInfo.setCullMode(vk::CullModeFlagBits::eFront);

        m_RasterizationInfo.setDepthBiasEnable(false)
            .setDepthClampEnable(false)
            .setDepthBiasConstantFactor(0.0f)
            .setDepthBiasSlopeFactor(0.0f)
            .setRasterizerDiscardEnable(false)
            .setPolygonMode(vk::PolygonMode::eFill);

        m_MultisampleCreateInfo.setSampleShadingEnable(false)
            .setRasterizationSamples(vk::SampleCountFlagBits::e1)
            .setMinSampleShading(1.f)
            .setPSampleMask(nullptr)
            .setAlphaToOneEnable(false)
            .setAlphaToCoverageEnable(false);

        m_BlendStateCreateInfo.setLogicOpEnable(false);
        m_BlendStateCreateInfo.setLogicOp(vk::LogicOp::eCopy);
        m_BlendStateCreateInfo.setPAttachments(nullptr);

        m_BlendStateCreateInfo.blendConstants[0] = .0f;
        m_BlendStateCreateInfo.blendConstants[1] = .0f;
        m_BlendStateCreateInfo.blendConstants[2] = .0f;
        m_BlendStateCreateInfo.blendConstants[3] = .0f;
    }
} // namespace VkCore
