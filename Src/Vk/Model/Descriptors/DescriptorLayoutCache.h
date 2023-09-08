#pragma once

#include <unordered_map>


#include "../Devices/Device.h"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_structs.hpp"

namespace VkCore
{
    class DescriptorLayoutCache
    {
      public:
        DescriptorLayoutCache(const Device& newDevice);
        void Cleanup();

        vk::DescriptorSetLayout CreateDescriptorLayout(const vk::DescriptorSetLayoutCreateInfo& createInfo);

        struct DescriptorLayoutInfo
        {
            // good idea to turn this into a inlined array
            std::vector<vk::DescriptorSetLayoutBinding> m_Bindings;

            bool operator==(const DescriptorLayoutInfo& other) const;

            size_t Hash() const;
        };

      private:
        struct DescriptorLayoutHash
        {

            std::size_t operator()(const DescriptorLayoutInfo& k) const
            {
                return k.Hash();
            }
        };

        std::unordered_map<DescriptorLayoutInfo, vk::DescriptorSetLayout, DescriptorLayoutHash> m_LayoutCache;
        Device m_Device;
    };

} // namespace VkCore
