#include <algorithm>
#include <vector>

#include "DescriptorAllocator.h"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_handles.hpp"
#include "vulkan/vulkan_structs.hpp"
#include "../../../Log/Log.h"

namespace VkCore
{

    DescriptorAllocator::DescriptorAllocator(const Device& logicalDevice)
    {
        m_Device = logicalDevice;
    }
    DescriptorAllocator::DescriptorAllocator(const Device& logicalDevice, int count,
                                             vk::DescriptorPoolCreateFlagBits flags, const PoolSizes& poolSizes)
    {
        m_Device = logicalDevice;
        m_UsedPools.emplace_back(CreatePool(count, flags, poolSizes));
    }

    vk::DescriptorPool DescriptorAllocator::CreatePool(int count, vk::DescriptorPoolCreateFlags flags,
                                                       const DescriptorAllocator::PoolSizes& poolSizes)
    {
        std::vector<vk::DescriptorPoolSize> vkSizes;
        vkSizes.reserve(poolSizes.m_Sizes.size());

        for (const auto& pair : poolSizes.m_Sizes)
        {
            vk::DescriptorPoolSize poolSize(pair.first, pair.second * count);
            vkSizes.emplace_back(poolSize);
        }

        vk::DescriptorPoolCreateInfo createInfo;

        createInfo.setPoolSizeCount(vkSizes.size()).setMaxSets(count).setPoolSizes(vkSizes).setFlags(flags);

        TRY_CATCH_BEGIN()

        return m_Device.CreateDescriptorPool(createInfo);

        TRY_CATCH_END()
    }

    void DescriptorAllocator::Cleanup()
    {
        for (const vk::DescriptorPool& pool : m_UsedPools)
        {
            m_Device.DestroyDescriptorPool(pool);
        }

        for (const vk::DescriptorPool& pool : m_FreePools)
        {
            m_Device.DestroyDescriptorPool(pool);
        }
    }

    vk::DescriptorPool DescriptorAllocator::GrabPool()
    {
        if (m_FreePools.size() > 0)
        {
            vk::DescriptorPool pool = m_FreePools.back();
            m_FreePools.pop_back();
            return pool;
        }

        return CreatePool(1000, vk::Flags<vk::DescriptorPoolCreateFlagBits>(0));
    }

    bool DescriptorAllocator::Allocate(vk::DescriptorSet& set, const vk::DescriptorSetLayout& layout)
    {

        if (!m_CurrentPool)
        {
            m_CurrentPool = GrabPool();
            m_UsedPools.push_back(m_CurrentPool);
        }

        vk::DescriptorSetAllocateInfo allocInfo;
        allocInfo.setDescriptorSetCount(1).setDescriptorPool(m_CurrentPool).setSetLayouts(layout).setPNext(nullptr);

        bool isReallocNeeded = false;

        try
        {
            std::vector<vk::DescriptorSet> newSets = m_Device.AllocateDescriptorSets(allocInfo);

            if (newSets.size() > 0)
                set = vk::DescriptorSet(newSets[0]);

            return true;
        }
        catch (const vk::SystemError& err)
        {
            switch (err.code().value())
            {
            case VK_ERROR_FRAGMENTED_POOL:
            case VK_ERROR_OUT_OF_POOL_MEMORY:
                isReallocNeeded = true;
                break;
            default:
                return false;
            }
        }

        if (isReallocNeeded)
        {
            m_CurrentPool = GrabPool();
            m_UsedPools.push_back(m_CurrentPool);
            allocInfo.setDescriptorPool(m_CurrentPool);

            TRY_CATCH_BEGIN()

            std::vector<vk::DescriptorSet> newSets = m_Device.AllocateDescriptorSets(allocInfo);

            // TODO: This might fail later with a nullptr!
            if (newSets.size() > 0)
                set = vk::DescriptorSet(newSets[0]);

            return true;

            TRY_CATCH_END()
        }
        return false;
    }

    void DescriptorAllocator::ResetPools()
    {
        for (const vk::DescriptorPool& pool : m_UsedPools)
        {
            m_Device.ResetDescriptorPool(pool);
        }

        m_FreePools = m_UsedPools;
        m_UsedPools.clear();

        m_CurrentPool = nullptr;
    }

} // namespace VkCore
