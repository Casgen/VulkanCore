#include "DescriptorBuilder.h"
#include "DescriptorAllocator.h"
#include "DescriptorLayoutCache.h"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_structs.hpp"
#include "../../Log/Log.h"

#include <algorithm>
#include <memory>

namespace VkCore
{

    DescriptorBuilder::DescriptorBuilder()
        : m_Cache(std::make_unique<DescriptorLayoutCache>()), m_Allocator(std::make_unique<DescriptorAllocator>())
    {
    }

    DescriptorBuilder::DescriptorBuilder(const Device& device)
        : m_Cache(std::make_unique<DescriptorLayoutCache>(device)),
          m_Allocator(std::make_unique<DescriptorAllocator>(device))
    {
    }

    DescriptorBuilder::~DescriptorBuilder()
    {
        // TODO: Check if this works!
        for (vk::WriteDescriptorSet& write : m_Writes)
        {
            delete[] write.pBufferInfo;
        }
    }

    DescriptorBuilder::DescriptorBuilder(DescriptorBuilder&& other)
    {
        if (this != &other)
        {
            m_Cache = std::move(other.m_Cache);
            m_Allocator = std::move(other.m_Allocator);
        }
    }

    DescriptorBuilder& DescriptorBuilder::operator=(DescriptorBuilder&& other)
    {
        if (this != &other)
        {
            m_Cache = std::move(other.m_Cache);
            m_Allocator = std::move(other.m_Allocator);
        }

        return *this;
    }

    DescriptorBuilder::DescriptorBuilder(std::unique_ptr<DescriptorLayoutCache> layoutCache,
                                         std::unique_ptr<DescriptorAllocator> allocator)
    {
        m_Cache = std::move(layoutCache);
        m_Allocator = std::move(allocator);
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

        for (vk::WriteDescriptorSet& write : m_Writes)
        {
            delete[] write.pBufferInfo;
        }

        m_Writes.clear();
    }

} // namespace VkCore
