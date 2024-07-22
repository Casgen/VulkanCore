#include "DescriptorBuilder.h"
#include "DescriptorAllocator.h"
#include "DescriptorLayoutCache.h"
#include "vulkan/vulkan_structs.hpp"
#include "../../Log/Log.h"

namespace VkCore
{

    DescriptorBuilder::DescriptorBuilder()
        : m_Cache(new DescriptorLayoutCache()), m_Allocator(new DescriptorAllocator())
    {
    }

    DescriptorBuilder::DescriptorBuilder(const Device& device)
        : m_Cache(new DescriptorLayoutCache(device)), m_Allocator(new DescriptorAllocator(device))
    {
    }

    DescriptorBuilder::DescriptorBuilder(DescriptorBuilder&& other)
    {
        if (this != &other)
        {
            m_Cache = other.m_Cache;
            m_Allocator = other.m_Allocator;
        }
    }

    DescriptorBuilder& DescriptorBuilder::operator=(DescriptorBuilder&& other)
    {
        if (this != &other)
        {
            m_Cache = other.m_Cache;
            m_Allocator = other.m_Allocator;
        }

        return *this;
    }

    DescriptorBuilder& DescriptorBuilder::BindImage(uint32_t binding, const vk::DescriptorImageInfo& imageInfo,
                                                    vk::DescriptorType type, vk::ShaderStageFlags stageFlags)
    {
        // create the descriptor binding for the layout
        vk::DescriptorSetLayoutBinding newBinding(binding, type, 1, stageFlags, nullptr);
        m_Bindings.push_back(newBinding);

        // create the descriptor writ
        vk::WriteDescriptorSet newWrite({}, binding, {}, 1, type, &imageInfo, nullptr, nullptr);

        m_Writes.push_back(newWrite);
        return *this;
    }

    bool DescriptorBuilder::Build(vk::DescriptorSet& set)
    {
        vk::DescriptorSetLayout layout;
        return Build(set, layout);
    }

    bool DescriptorBuilder::Build(vk::DescriptorSet& set, vk::DescriptorSetLayout& layout)
    {
        vk::DescriptorSetLayoutCreateInfo layoutInfo = vk::DescriptorSetLayoutCreateInfo();
        layoutInfo.pBindings = m_Bindings.data();
        layoutInfo.bindingCount = m_Bindings.size();

        layout = m_Cache->CreateDescriptorLayout(layoutInfo);

        bool success = m_Allocator->Allocate(set, layout);

        if (!success)
            return false;

        for (vk::WriteDescriptorSet& write : m_Writes)
        {
            write.dstSet = set;
        }

        TRY_CATCH_BEGIN()

        m_Allocator->m_Device.UpdateDescriptorSets(m_Writes);

        TRY_CATCH_END()

        return true;
    }

    void DescriptorBuilder::Clear()
    {
        m_Bindings.clear();
        m_Writes.clear();
    }

    void DescriptorBuilder::Cleanup()
    {
        m_Cache->Cleanup();
        m_Allocator->Cleanup();
    }

} // namespace VkCore
