#pragma once

#include <cstdint>
#include <map>
#include <vector>

#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_handles.hpp"
#include "vulkan/vulkan_structs.hpp"

#include "../Devices/Device.h"
#include "../../../Model/Shaders/ShaderData.h"
#include "../Vertex/VertexAttributeBuilder.h"

namespace VkCore
{
    class GraphicsPipelineBuilder
    {
      public:
        GraphicsPipelineBuilder(const Device& device) : m_Device(device)
        {
        }

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
         * to be defined in the index buffer to restart the assembly and that value is then defined when binding the Index buffer
         * with the vkCmdDrawIndexed function with it's vk::IndexType parameter. It is important to note that primitive restart is
         * not allowed for "list" topologies (triangle list, line list, etc.)
         */
        GraphicsPipelineBuilder& SetPrimitiveAssembly(const vk::PrimitiveTopology topology,
                                                      const bool isPrimitiveRestartEnabled = false);

        /**
         * @brief Adds a viewport specification
         * @param dimensions - X and Y floats describe the offset from the starting point of the window (bottom left corner???)
         * and Z a W describe the width and height.
         */
        GraphicsPipelineBuilder& AddViewport(const glm::uvec4& dimensions);

        /**
         * @brief Adds a viewport specification. Default offset is x = 0, y = 0
         * @param dimensions - describe the width and height of the viewport
         */
        GraphicsPipelineBuilder& AddViewport(const glm::uvec2& dimensions);

        /**
         * @brief Adds a provided viewport specification.
         * @param viewport
         */
        GraphicsPipelineBuilder& AddViewport(const vk::Viewport& viewport);

      private:
        Device m_Device;

        // Pipeline Creation structs
        vk::GraphicsPipelineCreateInfo m_PipelineCreateInfo{};

        std::vector<vk::PipelineShaderStageCreateInfo> m_ShaderStageCreateInfos{};

        std::vector<vk::VertexInputAttributeDescription> m_VertexInputAttributes;
        vk::VertexInputBindingDescription m_VertexInputBinding;
        vk::PipelineInputAssemblyStateCreateInfo m_VertexInputAssembly;

        std::vector<vk::Viewport> m_Viewports;
        std::vector<vk::Rect2D> m_Rect2Ds;

        vk::ShaderModule CreateShaderModule(const std::vector<uint32_t>& data);

        /**
         * @brief Binds the shader modules to the graphics pipeline.
         * @param shaderModulesMap - map of shader modules represented by binary data with a corresponding shader type
         * (kind)
         */
    };

} // namespace VkCore
