#include "DescriptorBuilder.h"
#include "DescriptorAllocator.h"
#include "DescriptorLayoutCache.h"

#include <algorithm>
#include <memory>

namespace VkCore
{
    DescriptorBuilder::DescriptorBuilder(std::unique_ptr<DescriptorLayoutCache> layoutCache,
                                         std::unique_ptr<DescriptorAllocator> allocator)
    {
        m_Cache = std::move(layoutCache);
        m_Allocator = std::move(allocator);
    }

    DescriptorBuilder& DescriptorBuilder::BindBuffer(uint32_t binding, const vk::DescriptorBufferInfo& bufferInfo,
                                                     vk::DescriptorType type, vk::ShaderStageFlags stageFlags)
    {
        // create the descriptor binding for the layout
        vk::DescriptorSetLayoutBinding newBinding(binding, type, 1, stageFlags, nullptr);
        m_Bindings.push_back(newBinding);

        // create the descriptor write
        vk::WriteDescriptorSet newWrite({}, binding, {}, 1, type, nullptr, &bufferInfo, nullptr);

        m_Writes.push_back(newWrite);
        return *this;
    }
    DescriptorBuilder& DescriptorBuilder::BindImage(uint32_t binding, const vk::DescriptorImageInfo& imageInfo,
                                                    vk::DescriptorType type, vk::ShaderStageFlags stageFlags)
    {
        // create the descriptor binding for the layout
        vk::DescriptorSetLayoutBinding newBinding(binding, type, 1, stageFlags, nullptr);
        m_Bindings.push_back(newBinding);

        // create the descriptor write
        vk::WriteDescriptorSet newWrite({}, binding, {}, 1, type, &imageInfo, nullptr, nullptr);

        m_Writes.push_back(newWrite);
        return *this;
    }

    bool DescriptorBuilder::Build(vk::DescriptorSet& set, vk::DescriptorSetLayout& layout)
    {
        vk::DescriptorSetLayoutCreateInfo layoutInfo;
        layoutInfo.setPNext(nullptr).setBindings(m_Bindings);

        layout = m_Cache->CreateDescriptorLayout(layoutInfo);

        bool success = m_Allocator->Allocate(set, layout);

        if (!success)
            return false;

        for (vk::WriteDescriptorSet& write : m_Writes)
        {
            write.setDstSet(set);
        }

        m_Allocator->m_Device.UpdateDescriptorSets(m_Writes);

        return true;
    }

    bool DescriptorBuilder::Build(vk::DescriptorSet& set)
    {
        vk::DescriptorSetLayoutCreateInfo layoutInfo;
        layoutInfo.setPNext(nullptr).setBindings(m_Bindings);

        bool success = m_Allocator->Allocate(set, m_Cache->CreateDescriptorLayout(layoutInfo));

        if (!success)
            return false;

        for (vk::WriteDescriptorSet& write : m_Writes)
        {
            write.setDstSet(set);
        }

        m_Allocator->m_Device.UpdateDescriptorSets(m_Writes);

        return true;
    }
} // namespace VkCore
