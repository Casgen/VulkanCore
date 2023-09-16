#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

#include "vk_mem_alloc.h"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_enums.hpp"

namespace VkCore
{

    class Buffer
    {
      public:
        Buffer() = default;
        ~Buffer();

        /**
         *  @brief Creates and allocates a new buffer.
         *  @param data - Pointer to a block of data to allocato on the buffer. Note that the data is being copied! 
         *  @param size - size of data in BYTES
         */
        Buffer(void* data, const size_t size, const vk::BufferUsageFlags& usageFlags);

        vk::BufferUsageFlags GetUsageFlags() const;
        uint32_t GetSize() const;
        vk::Buffer GetVkBuffer() const;
        VmaAllocation GetVmaAllocation() const;
        VmaAllocationInfo GetVmaAllocationInfo() const;

        void SetVkBuffer(const vk::Buffer& buffer);
        void SetVmaAllocation(const VmaAllocation& allocation);
        void SetVmaAllocationInfo(const VmaAllocationInfo& allocationInfo);
        void SetUsageFlags(const vk::BufferUsageFlags& flags);

      private:
        size_t m_Size = 0;

        vk::BufferUsageFlags m_UsageFlags{};
        vk::Buffer m_Buffer{};

        VmaAllocation m_Allocation{};
        VmaAllocationInfo m_AllocationInfo{};
    };
} // namespace VkCore
