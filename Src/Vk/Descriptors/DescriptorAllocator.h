#pragma once

#include <map>
#include <vector>

#include "../Devices/Device.h"
#include "vulkan/vulkan.hpp"

namespace VkCore
{
    class DescriptorAllocator
    {
      public:

        friend class DescriptorBuilder;

        struct PoolSizes
        {
            PoolSizes()
            {
            }

            std::map<vk::DescriptorType, float> m_Sizes = {{vk::DescriptorType::eSampler, 0.5f},
                                                           {vk::DescriptorType::eCombinedImageSampler, 4.f},
                                                           {vk::DescriptorType::eSampledImage, 4.f},
                                                           {vk::DescriptorType::eStorageImage, 1.f},
                                                           {vk::DescriptorType::eUniformTexelBuffer, 1.f},
                                                           {vk::DescriptorType::eStorageTexelBuffer, 1.f},
                                                           {vk::DescriptorType::eUniformBuffer, 2.f},
                                                           {vk::DescriptorType::eStorageBuffer, 2.f},
                                                           {vk::DescriptorType::eUniformBufferDynamic, 1.f},
                                                           {vk::DescriptorType::eStorageBufferDynamic, 1.f},
                                                           {vk::DescriptorType::eInputAttachment, 0.5f}};
        };

        DescriptorAllocator() : m_FreePools({}), m_UsedPools({}), m_Device(Device()) {}

        /**
         * @brief Initializes the object only with a logical device
         */
        DescriptorAllocator(const Device& logicalDevice);

        /**
         * @brief Initializes the object with a logical device and a new descriptor pool.
         * @param logicalDevice - a Vulkan logical device
         * @param count - how much memory should the descriptor pool reserve.
         * @param poolCreateFlags - flags indicating how the descriptor pool should be setup
         * @param poolSizes - a struct indicating how much memory should the descriptor pool reserve for each descriptor
         * type by a multiplier. (the individual descriptor type multipliers
         * from the pool sizes are multiplied by count param)
         */
        DescriptorAllocator(const Device& logicalDevice, int count, vk::DescriptorPoolCreateFlagBits flags,
                            const PoolSizes& poolSizes = PoolSizes());


        /**
         * @brief Creates a new descriptor pool
         * @param poolSizes - a struct indicating how much memory should the descriptor pool reserve for each descriptor
         * type by a multiplier.
         * @param count - how much memory should the descriptor pool reserve (the individual descriptor type multipliers
         * from the pool sizes are multiplied by this param).
         * @param poolCreateFlags - flags indicating how the descriptor pool should be setup
         */
        vk::DescriptorPool CreatePool(int count, vk::DescriptorPoolCreateFlags flags, const PoolSizes& poolSizes = PoolSizes());


        /**
        * @brief Frees all the used and free pools.
        */
        void Cleanup();

        /**
        * @brief Obtains a first available descriptor pool.
        */
        vk::DescriptorPool GrabPool();

        /**
        * @brief Allocates the given descriptor set with its resources.
        * 
        */
        bool Allocate(vk::DescriptorSet& set, const vk::DescriptorSetLayout& layout);


        /**
        * @brief Resets the used pools and sets them ass free pools. Resulting in resources being freed.
        */
        void ResetPools();

      private:
        std::vector<vk::DescriptorPool> m_UsedPools;
        std::vector<vk::DescriptorPool> m_FreePools;

        vk::DescriptorPool m_CurrentPool;

        Device m_Device;

    };

} // namespace VkCore
