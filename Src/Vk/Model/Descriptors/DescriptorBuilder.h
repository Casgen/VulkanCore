#pragma once

#include <memory>

#include "../Devices/Device.h"
#include "../Buffers/Buffer.h"
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

        DescriptorBuilder() = default;
        ~DescriptorBuilder();
        

        /**
         *   @brief Initializes the DescriptorBuilder with the DescriptorAllocator and DescriptorLayoutCache.
         *   @brief device
         */
        DescriptorBuilder(const Device& device);

        /**
         *   @brief Initializes the DescriptorBuilder with the given DescriptorAllocator and DescriptorLayoutCache.
         *   @param layoutCache - a unique pointer to the layoutCache. Use std::move()!
         *   @param allocator - a unique pointer to the layoutCache. Use std::move()!
         */
        DescriptorBuilder(DescriptorLayoutCache* layoutCache,
                          DescriptorAllocator* allocator);

        DescriptorBuilder& BindBuffer(uint32_t binding, const vk::DescriptorBufferInfo& bufferInfo,
                                      vk::DescriptorType type, vk::ShaderStageFlags stageFlags);

        DescriptorBuilder& BindBuffer(uint32_t binding, const Buffer& buffer,
                                      vk::DescriptorType type, vk::ShaderStageFlags stageFlags);

        DescriptorBuilder& BindBuffer(uint32_t binding, const Buffer& buffer,
                                      vk::DescriptorType type, vk::ShaderStageFlags stageFlags, const vk::DeviceSize offset, const vk::DeviceSize range);


        DescriptorBuilder& BindImage(uint32_t binding, const vk::DescriptorImageInfo& imageInfo,
                                     vk::DescriptorType type, vk::ShaderStageFlags stageFlags);

        bool Build(vk::DescriptorSet& set, vk::DescriptorSetLayout& layout);
        bool Build(vk::DescriptorSet& set);

      private:
        std::vector<vk::WriteDescriptorSet> m_Writes = {};
        std::vector<vk::DescriptorSetLayoutBinding> m_Bindings = {};

        // The Descriptor builder takes ownership of the DescriptorLayoutCache's a DescriptorAllocator's memory
        // Therefore it is responsible for their deletion.

        DescriptorLayoutCache* m_Cache = nullptr;
        DescriptorAllocator* m_Allocator = nullptr;
    };

} // namespace VkCore
