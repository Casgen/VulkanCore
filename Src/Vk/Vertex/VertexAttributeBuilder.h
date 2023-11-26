
#pragma once

#include "vulkan/vulkan.hpp"
#include <cstdint>
#include "glm/fwd.hpp"

namespace VkCore
{
    class VertexAttributeBuilder
    {

      public:
        /**
         * @brief Constructs a VertexAttributeBuilder object for specifying the layout of a custom vertex.
         * Note that by default input rate is Vertex and the binding is set to 0!
         */
        VertexAttributeBuilder() : m_Descriptions({})
        {
            m_Binding.stride = 0;
            m_Binding.binding = 0;
            m_Binding.inputRate = vk::VertexInputRate::eVertex;
        }

        /**
         * @brief Tells vulkan at what rate should be the data inserted into the vertex shader. It can be either
         * per-vertex data or per instance.
         */
        VertexAttributeBuilder& SetInputRate(const vk::VertexInputRate inputRate)
        {
            m_Binding.setInputRate(inputRate);
            return *this;
        }

        /**
         * @brief At what binding index should the vertex buffer be bound to in the vertex shader.
         */
        VertexAttributeBuilder& SetBinding(const uint32_t binding)
        {
            m_Binding.setBinding(binding);

            for (auto& desc : m_Descriptions)
            {
                desc.binding = binding;
            }

            return *this;
        }

        std::vector<vk::VertexInputAttributeDescription> GetAttributeDescriptions() const
        {
            return m_Descriptions;
        }

        vk::VertexInputBindingDescription GetBindingDescription() const
        {
            return m_Binding;
        }

        void Reset()
        {
            m_Binding.setBinding(0);
            m_Binding.setStride(0);
            m_Binding.setInputRate(vk::VertexInputRate::eVertex);

            m_Descriptions.clear();
        }

        // ------------- ATTRIBUTES ----------------

        /**
         * @brief Pushes onto a stack the vertex attribute. With it its location index is automatically incremented.
         */
        template <typename T>
        VertexAttributeBuilder& PushAttribute(const uint32_t count);

      private:
        std::vector<vk::VertexInputAttributeDescription> m_Descriptions;
        vk::VertexInputBindingDescription m_Binding;

        // ------------- ATTRIBUTES ----------------

        template <typename T>
        vk::Format GetFormat(const uint32_t count)
        {
            return vk::Format::eUndefined;
        }
    };

} // namespace VkCore
