#include <cstddef>
#include <cstdint>
#include <sys/types.h>

#include "DescriptorLayoutCache.h"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_handles.hpp"
#include "vulkan/vulkan_structs.hpp"
#include "vulkan/vulkan_enums.hpp"

#include "../../Log/Log.h"

namespace VkCore
{

    DescriptorLayoutCache::DescriptorLayoutCache(const Device& device) : m_Device(device), m_LayoutCache({})
    {
    }

    vk::DescriptorSetLayout DescriptorLayoutCache::CreateDescriptorLayout(
        const vk::DescriptorSetLayoutCreateInfo& createInfo)
    {

        DescriptorLayoutInfo layoutInfo;

        layoutInfo.m_Bindings.reserve(createInfo.bindingCount);

        bool isSorted = true;
        int lastBinding = -1;

        // copy from the direct info struct into our own one
        for (int i = 0; i < createInfo.bindingCount; i++)
        {
            layoutInfo.m_Bindings.emplace_back(createInfo.pBindings[i]);

            // check that the bindings are in strict increasing order
            if (static_cast<int>(createInfo.pBindings[i].binding) > lastBinding)
            {
                lastBinding = createInfo.pBindings[i].binding;
            }
            else
            {
                isSorted = false;
            }
        }
        // sort the bindings if they aren't in order
        if (!isSorted)
        {
            std::sort(
                layoutInfo.m_Bindings.begin(), layoutInfo.m_Bindings.end(),
                [](VkDescriptorSetLayoutBinding& a, VkDescriptorSetLayoutBinding& b) { return a.binding < b.binding; });
        }

        auto it = m_LayoutCache.find(layoutInfo);

        if (it != m_LayoutCache.end())
        {
            return it->second;
        }

        vk::DescriptorSetLayout layout;

        TRY_CATCH_BEGIN()
        // create a new one (not found)
        layout = m_Device.CreateDescriptorSetLayout(createInfo);

        TRY_CATCH_END()

        // add to cache
        m_LayoutCache[layoutInfo] = layout;
        return layout;
    }

    void DescriptorLayoutCache::Cleanup()
    {
        for (const auto& pair : m_LayoutCache)
        {
            m_Device.DestroyDescriptorSetLayout(pair.second);
        }
    }

    bool DescriptorLayoutCache::DescriptorLayoutInfo::operator==(const DescriptorLayoutInfo& other) const
    {
        if (other.m_Bindings.size() != m_Bindings.size())
        {
            return false;
        }
        else
        {
            // compare each of the bindings is the same. Bindings are sorted so they will match
            for (int i = 0; i < m_Bindings.size(); i++)
            {
                if (other.m_Bindings[i].binding != m_Bindings[i].binding)
                    return false;

                if (other.m_Bindings[i].descriptorType != m_Bindings[i].descriptorType)
                    return false;

                if (other.m_Bindings[i].descriptorCount != m_Bindings[i].descriptorCount)
                    return false;

                if (other.m_Bindings[i].stageFlags != m_Bindings[i].stageFlags)
                    return false;
            }
            return true;
        }
    }

    size_t DescriptorLayoutCache::DescriptorLayoutInfo::Hash() const
    {
        size_t result = std::hash<size_t>()(m_Bindings.size());

        for (const vk::DescriptorSetLayoutBinding& b : m_Bindings)
        {

            size_t descriptorType = static_cast<uint32_t>(b.descriptorType);
            size_t stageFlags = static_cast<uint32_t>(b.stageFlags);

            size_t bindingHash = b.binding | descriptorType << 16 | b.descriptorCount << 24 | stageFlags << 32;

            result ^= std::hash<size_t>()(bindingHash);
        }

        return result;
    }
} // namespace VkCore
