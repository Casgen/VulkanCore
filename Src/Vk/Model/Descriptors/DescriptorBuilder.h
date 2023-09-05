#pragma once

#include <memory>

#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_handles.hpp"
#include "vulkan/vulkan_enums.hpp"

namespace VkCore
{
    class DescriptorAllocator;
    class DescriptorLayoutCache;

    class DescriptorBuilder
    {
      public:
        DescriptorBuilder(std::unique_ptr<DescriptorLayoutCache> layoutCache, std::unique_ptr<DescriptorAllocator> allocator);

        DescriptorBuilder& BindBuffer(uint32_t binding, const vk::DescriptorBufferInfo& bufferInfo, vk::DescriptorType type,
                                      vk::ShaderStageFlags stageFlags);
        DescriptorBuilder& BindImage(uint32_t binding, const vk::DescriptorImageInfo& imageInfo, vk::DescriptorType type,
                                     vk::ShaderStageFlags stageFlags);

        bool Build(vk::DescriptorSet& set, vk::DescriptorSetLayout& layout);
        bool Build(vk::DescriptorSet& set);

      private:
        std::vector<vk::WriteDescriptorSet> m_Writes;
        std::vector<vk::DescriptorSetLayoutBinding> m_Bindings;

        std::unique_ptr<DescriptorLayoutCache> m_Cache;
        std::unique_ptr<DescriptorAllocator> m_Allocator;
    };

} // namespace VkCore
