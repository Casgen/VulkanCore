#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "vulkan/vulkan.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "../../Model/Shaders/ShaderData.h"
#include "../Devices/Device.h"
#include "../Vertex/VertexAttributeBuilder.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_structs.hpp"

namespace VkCore
{

    // TODO: Modify this class to use the DeviceManager.
    class GraphicsPipelineBuilder
    {

      public:
        GraphicsPipelineBuilder(bool isMeshShading = false) : m_IsMeshShading(isMeshShading)
        {
            Init();
        }

        GraphicsPipelineBuilder(const Device& device, bool isMeshShading = false)
            : m_Device(device), m_IsMeshShading(isMeshShading)
        {
            Init();
        }

        void SetDevice(const Device& device)
        {
            m_Device = device;
        }

        // ---------------- SHADERS -----------------

        /**
         * @brief Create vk::ShaderModules and binds it to the graphics pipeline
         * @param shaders
         */
        GraphicsPipelineBuilder& BindShaderModules(const std::vector<ShaderData>& shaders);

        /**
         * @brief Create a vk::ShaderModule and binds it to the graphics pipeline
         * @param shaderData
         */
        GraphicsPipelineBuilder& BindShaderModule(const ShaderData& shaderData);

        // -------------------- VERTICES ----------------------

        /**
         * @brief Sets a binding between the vertex attributes and the graphics pipeline. NOTE THAT THIS METHOD BY
         * DEFAULT RESETS THE BUILDER TO ITS INITIAL STATE!
         *
         * @param vertexAttributeBuilder - Reference to the vertexAttributeBuilder. It is expected that the builder
         * already has some information built into it.
         * @param reset - Determines whether the builder should be reset to its initial state (All descriptions reset
         * and clear).
         */
        GraphicsPipelineBuilder& BindVertexAttributes(VertexAttributeBuilder& vertexAttributeBuilder,
                                                      const bool resetBuilder = true);

        /**
         * @brief Sets up the Vertex input assembly based on the topology.
         * @param topology - enumerator describing how vertices should be assembled to create polygons.
         * @param isPrimitiveRestartEnabled - Enables Vulkan to restart the assembly in the middle of the process. This
         * can be used for planes where at the sides the polygons need to be wrapped. The triggering value has
         * to be defined in the index buffer to restart the assembly and that value is then defined when binding the
         * Index buffer with the vkCmdDrawIndexed function with it's vk::IndexType parameter. It is important to note
         * that primitive restart is not allowed for "list" topologies (triangle list, line list, etc.)
         */
        GraphicsPipelineBuilder& SetPrimitiveAssembly(const vk::PrimitiveTopology topology,
                                                      const bool isPrimitiveRestartEnabled = false);

        // ------- VIEWPORTS AND SCISSORS -----------

        /**
         * @brief Adds a viewport specification
         * @param viewportDimensions - X and Y floats describe the offset of the viewpoer from the starting point of the
         * window (bottom left corner???) and Z a W describe the width and height.
         * @param scissorsDimensions - X and Y floats describe the offset from the starting point of the window (bottom
         * left corner???) and Z a W describe the width and height. This is used for cropping the viewport.
         */
        GraphicsPipelineBuilder& AddViewport(const glm::uvec4& viewportDimension, const glm::uvec4& scissorsDimensions);

        /**
         * @brief Adds a viewport specification. Default offset is x = 0, y = 0
         * @param viewportDimensions - describe the width and height of the viewport
         * @param viewportDimensions - describe the width and height of the scissor cropping the viewport
         */
        GraphicsPipelineBuilder& AddViewport(const glm::uvec2& viewportDimensions,
                                             const glm::uvec2& scissorsDimensions);

        /**
         * @brief Adds a viewport specification. Default offset is x = 0, y = 0
         * @param viewportDimensions - describe the width and height of the viewport. By default the scissor has the
         * same dimension as the viewport
         */
        GraphicsPipelineBuilder& AddViewport(const glm::uvec2& viewportDimensions);

        /**
         * @brief Adds a viewport specification. Default offset is x = 0, y = 0
         * @param viewportDimensions - describe the width and height of the viewport. By default the scissor has the
         * same dimension as the viewport
         */
        GraphicsPipelineBuilder& AddViewport(const glm::uvec4& viewportDimensions);

        /**
         * @brief Adds a provided viewport specification.
         */
        GraphicsPipelineBuilder& AddViewport(const vk::Viewport& viewport);

        /**
         * @brief Adds a provided viewport and scissor specification.
         */
        GraphicsPipelineBuilder& AddViewport(const vk::Viewport& viewport, const vk::Rect2D& scissor);

        /**
         * @brief Adds a dynamic state which enables the flexibility to change the given state during drawing commands.
         * Note that the state has to be always set during the drawing stage! If not, the graphics pipeline can
         * misbehave!
         */
        GraphicsPipelineBuilder& AddDynamicState(const vk::DynamicState dynamicState);

        // ------------------ RASTERIZATION ------------------------

        /**
         * @brief If Depth clamping is enabled, then fragments that are beyond the near and far planes are clamped to
         * them as opposed to discarding them. This is useful in some special cases like shadow maps. Using this
         * requires enabling a GPU feature.
         */
        GraphicsPipelineBuilder& EnableDepthClamping(const bool isEnabled);

        GraphicsPipelineBuilder& SetDepthStencilState(const vk::PipelineDepthStencilStateCreateInfo& createInfo);

        /**
         * Enables depth and stencil testing in the pipeline.
         */
        GraphicsPipelineBuilder& EnableDepthTest();

        /**
         * Enables stencil testing to the pipeline. MAKE SURE YOU HAVE A DEPTH IMAGE FORMAT THAT SUPPORTS STENCIL!
         */
        GraphicsPipelineBuilder& EnableStencilTest(const vk::StencilOpState backState,
                                                   const vk::StencilOpState frontState);

        /**
         * @brief tells vulkan how to polygons should be drawn (Fill, lines, or as only their points).
         * By default it is set to vk::PolygonMode:eFill.
         */
        GraphicsPipelineBuilder& SetPolygonMode(const vk::PolygonMode polygonMode);

        /**
         * @brief Tells Vulkan whether the input geometry should be disarded or not. By default set to false.
         */
        GraphicsPipelineBuilder& EnabledDiscard(const bool isEnabled);

        /**
         * @brief tells Vulkan what direction should be considered as front face when vertices are
         * forming polygons (Counter clockwise or clockwise). By default it is set to Clockwise.
         */
        GraphicsPipelineBuilder& FrontFaceDirection(const vk::FrontFace frontFace);

        GraphicsPipelineBuilder& SetCullMode(const vk::CullModeFlags cullMode);

        /**
         * @brief Sets the thickness of the lines when Polygons are drawn with lines. By default it is
         * set to 1.f
         * */
        GraphicsPipelineBuilder& SetLineWidth(const float lineWidth = 1.0f);

        // ------------------ BLENDING ----------------------

        /**
         * @brief tells Vulkan to enable transformation of depth values. By default it is false.
         */
        GraphicsPipelineBuilder& EnableDepthBias(const bool isEnabled);

        /**
         * @brief sets the parameters for depth bias (transformation of the depth values)
         * @param constantFactor - this value will be added to the depth value
         * @param clamp - boundary for clamping the value
         * @param slopeFactor */
        GraphicsPipelineBuilder& SetDepthBiasParams(const float constantFactor = 0.0f, const float clamp = 0.0f,
                                                    const float slopeFactor = 0.0f);

        /**
         * @brief Adds a customized color blend attachment state. If you don't want any blending for the
         * attachment, use the @code AddDisabledBlendAttachment @endcode method
         * */
        GraphicsPipelineBuilder& AddBlendAttachment(const vk::PipelineColorBlendAttachmentState blendAttachment);

        /**
         * @brief Adds a color blend attachment state which is automatically disables blending. This is required to do
         * for every attchment in the Render Pass!
         */
        GraphicsPipelineBuilder& AddDisabledBlendAttachment();

        // ---------------------- PIPELINE LAYOUT -------------------------

        /**
         * @brief Appends the given descriptor set layouts.
         */
        GraphicsPipelineBuilder& AddDescriptorLayout(const std::vector<vk::DescriptorSetLayout> layouts);

        /**
         * @brief Appends the given descriptor set layout.
         */
        GraphicsPipelineBuilder& AddDescriptorLayout(const vk::DescriptorSetLayout layout);

        /**
         * @brief Sets the descriptor set layouts. Note that this will discard the previously set layouts!
         */
        GraphicsPipelineBuilder& SetDescriptorLayout(const std::vector<vk::DescriptorSetLayout>& layouts);

        /**
         * Sets up Push constants (uniform type variables in shaders).
         * @param offset - Indicates the offset into the variable (offset in bytes from which the variable should be
         * read). By default is 0
         * @param stageFlags - Where the push constant will be used.
         */
        template <typename T>
        GraphicsPipelineBuilder& AddPushConstantRange(const vk::ShaderStageFlags stageFlags, const uint32_t offset = 0)
        {

            vk::PushConstantRange range{};

            range.offset = offset;
            range.stageFlags = stageFlags;
            range.size = sizeof(T);

            m_PushConstantRanges.push_back(std::move(range));

            return *this;
        }

        /**
         * Sets up Push constants (uniform type variables in shaders).
         */
        GraphicsPipelineBuilder& AddPushConstantRange(const vk::PushConstantRange& range);

        GraphicsPipelineBuilder& BindRenderPass(const vk::RenderPass& renderPass);

        /**
         * @brief Gathers all the data, sets all the necessary pipeline states and creates a new graphics pipeline.
         * @return Vulkan pipeline object;
         */
        vk::Pipeline Build();

        /**
         * @brief Gathers all the data, sets all the necessary pipeline states and layots and builds a new graphics
         * pipeline and its layout.
         * @return Vulkan pipeline object;
         */
        vk::Pipeline Build(vk::PipelineLayout& pipelineLayout);

      private:
        vk::ShaderModule CreateShaderModule(const std::vector<uint32_t>& data);

        // ------------------ VALIDATION METHODS --------------------

        void Init();

        void ValidateShaderInfo();
        bool ValidateVertexInfo();
        void ValidateViewportInfo();

        Device m_Device;

        // Pipeline Creation structs
        vk::GraphicsPipelineCreateInfo m_PipelineCreateInfo{};

        std::vector<vk::PipelineShaderStageCreateInfo> m_ShaderStageCreateInfos{};

        std::vector<vk::VertexInputAttributeDescription> m_VertexInputAttributes{};
        vk::VertexInputBindingDescription m_VertexInputBinding{};
        vk::PipelineInputAssemblyStateCreateInfo m_VertexInputAssembly{};

        std::vector<vk::Viewport> m_Viewports{};
        std::vector<vk::Rect2D> m_Scissors{};

        std::vector<vk::DynamicState> m_DynamicStates;

        vk::PipelineRasterizationStateCreateInfo m_RasterizationInfo{};
        vk::PipelineMultisampleStateCreateInfo m_MultisampleCreateInfo{};
        vk::PipelineDepthStencilStateCreateInfo m_DepthStencilState{};

        std::vector<vk::PipelineColorBlendAttachmentState> m_ColorBlendAttachmentStates{};
        vk::PipelineColorBlendStateCreateInfo m_BlendStateCreateInfo{};

        std::vector<vk::DescriptorSetLayout> m_DescriptorSetLayouts{};
        std::vector<vk::PushConstantRange> m_PushConstantRanges{};

        bool m_IsMeshShading;
    };

    class ComputePipelineBuilder
    {
      public:
        ComputePipelineBuilder() {};

        ComputePipelineBuilder& BindShaderModule(const ShaderData& shaderData);

        /**
         * @brief Appends the given descriptor set layout.
         */
        ComputePipelineBuilder& AddDescriptorLayout(const vk::DescriptorSetLayout& layout);

        template <typename T>
        ComputePipelineBuilder& AddPushConstantRange(const vk::ShaderStageFlags stageFlags, const uint32_t offset = 0)
        {

            vk::PushConstantRange range{};

            range.offset = offset;
            range.stageFlags = stageFlags;
            range.size = sizeof(T);

            m_PushConstantRanges.push_back(std::move(range));

            return *this;
        }

        vk::Pipeline Build(vk::PipelineLayout& pipelineLayout);

		void Reset();

      private:
		bool m_ShaderWasBound = false;
        vk::PipelineShaderStageCreateInfo m_ShaderStageCreateInfo{};
        std::vector<vk::DescriptorSetLayout> m_DescriptorSetLayouts{};
        std::vector<vk::PushConstantRange> m_PushConstantRanges{};
    };

} // namespace VkCore
