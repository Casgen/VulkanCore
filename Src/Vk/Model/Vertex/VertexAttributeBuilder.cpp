#include "VertexAttributeBuilder.h"

namespace VkCore
{

    template <typename T>
    VertexAttributeBuilder& VertexAttributeBuilder::PushAttribute(const uint32_t count)
    {
        LOG(Vulkan, Error, "The attribute format was not recognized! No attribute has been pushed!")
        return *this;
    }

    template <>
    vk::Format VertexAttributeBuilder::GetFormat<float>(const uint32_t count)
    {
        switch (count)
        {
        case 1:
            return vk::Format::eR32Sfloat;
            break;
        case 2:
            return vk::Format::eR32G32Sfloat;
            break;
        case 3:
            return vk::Format::eR32G32B32Sfloat;
            break;
        case 4:
            return vk::Format::eR32G32B32A32Sfloat;
            break;
        case 0: {
            LOG(Vulkan, Error, "Couldn't recognize the given Vertex format! count is 0! returning eUndefined.")
            return vk::Format::eUndefined;
        }
        default: {
            LOG(Vulkan, Error, "Couldn't recognize the given Vertex format! returning eUndefined")
            return vk::Format::eUndefined;
            break;
        }
        }
    }

    template <>
    vk::Format VertexAttributeBuilder::GetFormat<uint32_t>(const uint32_t count)
    {
        switch (count)
        {
        case 1:
            return vk::Format::eR32Uint;
            break;
        case 2:
            return vk::Format::eR32G32Uint;
            break;
        case 3:
            return vk::Format::eR32G32B32Uint;
            break;
        case 4:
            return vk::Format::eR32G32B32A32Uint;
            break;
        case 0: {
            LOG(Vulkan, Error, "Couldn't recognize the given Vertex format! count is 0! returning eUndefined.")
            return vk::Format::eUndefined;
        }
        default: {
            LOG(Vulkan, Error, "Couldn't recognize the given Vertex format! returning eUndefined")
            return vk::Format::eUndefined;
            break;
        }
        }
    }

    template <>
    inline vk::Format VertexAttributeBuilder::GetFormat<int32_t>(const uint32_t count)
    {
        switch (count)
        {
        case 1:
            return vk::Format::eR32Sint;
            break;
        case 2:
            return vk::Format::eR32G32Sint;
            break;
        case 3:
            return vk::Format::eR32G32B32Sint;
            break;
        case 4:
            return vk::Format::eR32G32B32A32Sint;
            break;
        case 0: {
            LOG(Vulkan, Error, "Couldn't recognize the given Vertex format! count is 0! returning eUndefined.");
            return vk::Format::eUndefined;
        }
        default: {
            LOG(Vulkan, Error, "Couldn't recognize the given Vertex format! returning eUndefined");
            return vk::Format::eUndefined;
            break;
        }
        }
    }

    template <>
    inline vk::Format VertexAttributeBuilder::GetFormat<double>(const uint32_t count)
    {
        switch (count)
        {
        case 1:
            return vk::Format::eR64Sfloat;
            break;
        case 2:
            return vk::Format::eR64G64Sfloat;
            break;
        case 3:
            return vk::Format::eR64G64B64Sfloat;
            break;
        case 4:
            return vk::Format::eR64G64B64A64Sfloat;
            break;
        case 0: {
            LOG(Vulkan, Error, "Couldn't recognize the given Vertex format! count is 0! returning eUndefined.");
            return vk::Format::eUndefined;
        }
        default: {
            LOG(Vulkan, Error, "Couldn't recognize the given Vertex format! returning eUndefined")
            return vk::Format::eUndefined;
            break;
        }
        }
    }

    template <>
    inline vk::Format VertexAttributeBuilder::GetFormat<unsigned char>(uint32_t count)
    {
        switch (count)
        {
        case 1:
            return vk::Format::eR8Sint;
            break;
        case 2:
            return vk::Format::eR8G8Sint;
            break;
        case 3:
            return vk::Format::eR8G8B8Sint;
            break;
        case 4:
            return vk::Format::eR8G8B8A8Sint;
            break;
        case 0: {
            LOG(Vulkan, Error, "Couldn't recognize the given Vertex format! count is 0! returning eUndefined.")
            return vk::Format::eUndefined;
        }
        default: {
            LOG(Vulkan, Error, "Couldn't recognize the given Vertex format! returning eUndefined")
            return vk::Format::eUndefined;
            break;
        }
        }
    }

    // The Push Attributes implementation has to be put down below the Get
    template <>
    VertexAttributeBuilder& VertexAttributeBuilder::PushAttribute<float>(const uint32_t count)
    {
        vk::VertexInputAttributeDescription description{};
        description.setFormat(GetFormat<float>(count));
        description.setOffset(m_Binding.stride);
        description.setBinding(0);
        description.setLocation(m_Descriptions.size());

        m_Descriptions.emplace_back(description);
        m_Binding.stride += sizeof(float) * count;

        return *this;
    }

    template <>
    VertexAttributeBuilder& VertexAttributeBuilder::PushAttribute<uint32_t>(const uint32_t count)
    {
        vk::VertexInputAttributeDescription description{};
        description.setFormat(GetFormat<uint32_t>(count));
        description.setOffset(m_Binding.stride);
        description.setBinding(0);
        description.setLocation(m_Descriptions.size());

        m_Descriptions.emplace_back(description);
        m_Binding.stride += sizeof(uint32_t) * count;

        return *this;
    }

    template <>
    VertexAttributeBuilder& VertexAttributeBuilder::PushAttribute<int32_t>(const uint32_t count)
    {
        vk::VertexInputAttributeDescription description{};
        description.setFormat(GetFormat<int32_t>(count));
        description.setOffset(m_Binding.stride);
        description.setBinding(0);
        description.setLocation(m_Descriptions.size());

        m_Descriptions.emplace_back(description);
        m_Binding.stride += sizeof(int32_t) * count;

        return *this;
    }

    template <>
    VertexAttributeBuilder& VertexAttributeBuilder::PushAttribute<double>(const uint32_t count)
    {
        vk::VertexInputAttributeDescription description{};
        description.setFormat(GetFormat<double>(count));
        description.setOffset(m_Binding.stride);
        description.setBinding(0);
        description.setLocation(m_Descriptions.size());

        m_Descriptions.emplace_back(description);
        m_Binding.stride += sizeof(int32_t) * count;

        return *this;
    }

    template <>
    VertexAttributeBuilder& VertexAttributeBuilder::PushAttribute<unsigned char>(const uint32_t count)
    {
        vk::VertexInputAttributeDescription description{};
        // description.setFormat(GetFormat<unsigned char>(count));
        description.setOffset(m_Binding.stride);
        description.setBinding(0);
        description.setLocation(m_Descriptions.size());

        m_Descriptions.emplace_back(description);
        m_Binding.stride += sizeof(int32_t) * count;

        return *this;
    }

    // GLM Float vectors

    template <>
    VertexAttributeBuilder& VertexAttributeBuilder::PushAttribute<glm::vec1>(const uint32_t count)
    {
        return PushAttribute<float>(1);
    }

    template <>
    VertexAttributeBuilder& VertexAttributeBuilder::PushAttribute<glm::vec2>(const uint32_t count)
    {
        return PushAttribute<float>(2);
    }

    template <>
    VertexAttributeBuilder& VertexAttributeBuilder::PushAttribute<glm::vec3>(const uint32_t count)
    {
        return PushAttribute<float>(3);
    }

    template <>
    VertexAttributeBuilder& VertexAttributeBuilder::PushAttribute<glm::vec4>(const uint32_t count)
    {
        return PushAttribute<float>(4);
    }

    // GLM Signed integer floats

    template <>
    VertexAttributeBuilder& VertexAttributeBuilder::PushAttribute<glm::ivec1>(const uint32_t count)
    {
        return PushAttribute<int32_t>(1);
    }

    template <>
    VertexAttributeBuilder& VertexAttributeBuilder::PushAttribute<glm::ivec2>(const uint32_t count)
    {
        return PushAttribute<int32_t>(2);
    }

    template <>
    VertexAttributeBuilder& VertexAttributeBuilder::PushAttribute<glm::ivec3>(const uint32_t count)
    {
        return PushAttribute<int32_t>(3);
    }

    template <>
    VertexAttributeBuilder& VertexAttributeBuilder::PushAttribute<glm::ivec4>(const uint32_t count)
    {
        return PushAttribute<int32_t>(4);
    }

    // GLM Signed doubles
    template <>
    VertexAttributeBuilder& VertexAttributeBuilder::PushAttribute<glm::dvec1>(const uint32_t count)
    {
        return PushAttribute<double>(1);
    }

    template <>
    VertexAttributeBuilder& VertexAttributeBuilder::PushAttribute<glm::dvec2>(const uint32_t count)
    {
        return PushAttribute<double>(2);
    }

    template <>
    VertexAttributeBuilder& VertexAttributeBuilder::PushAttribute<glm::dvec3>(const uint32_t count)
    {
        return PushAttribute<double>(3);
    }

    template <>
    VertexAttributeBuilder& VertexAttributeBuilder::PushAttribute<glm::dvec4>(const uint32_t count)
    {
        return PushAttribute<double>(4);
    }
} // namespace VkCore
