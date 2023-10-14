#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_enums.hpp"
#include "vk_mem_alloc.h"

namespace VkCore
{

    class Buffer
    {
      public:
        struct BufferInfo
        {
            size_t m_Size = 0;
            vk::BufferUsageFlags m_UsageFlags = {};
            VmaAllocationCreateFlags m_AllocCreateFlags = {};
            VmaMemoryUsage m_MemoryUsage = {};
            vk::BufferCreateFlags m_CreateFlags = {};
            std::vector<uint32_t> m_QueueFamilyIndices = {};

            BufferInfo() = default;

            BufferInfo(const size_t size, const vk::BufferUsageFlags usageFlags,
                       const VmaAllocationCreateFlags allocCreateFlags,
                       const VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO)
                : m_Size(size), m_UsageFlags(usageFlags), m_AllocCreateFlags(allocCreateFlags),
                  m_MemoryUsage(memoryUsage)
            {
            }

            BufferInfo(const size_t size, const vk::BufferUsageFlags usageFlags,
                       const VmaAllocationCreateFlags allocCreateFlags, const vk::BufferCreateFlags createFlags,
                       const VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO)
                : m_Size(size), m_UsageFlags(usageFlags), m_AllocCreateFlags(allocCreateFlags),
                  m_MemoryUsage(memoryUsage), m_CreateFlags(createFlags)
            {
            }

            BufferInfo(const size_t size, const vk::BufferUsageFlags usageFlags,
                       const VmaAllocationCreateFlags allocCreateFlags, const vk::BufferCreateFlags createFlags,
                       const std::vector<uint32_t>& queueFamilyIndices,
                       const VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO)
                : m_Size(size), m_UsageFlags(usageFlags), m_AllocCreateFlags(allocCreateFlags),
                  m_MemoryUsage(memoryUsage), m_CreateFlags(createFlags), m_QueueFamilyIndices(queueFamilyIndices)
            {
            }
        };

        Buffer() = default;
        ~Buffer();

        /**
         *  @brief Creates a buffer object. Note that no vulkan buffer has been created and allocated yet! Call after
         * this your desired initialization function!
         */
        Buffer(const vk::BufferUsageFlags& usageFlags) : m_UsageFlags(usageFlags)
        {
        }

        // ----------- INITIALIZATION -----------------

        /**
         * @brief Allocates a new buffer with the given data and puts it on the GPU. The buffer won't be visible
         * to the host (CPU)!
         * @param data - Pointer to a block of data to allocate on the buffer. Note that the data is being copied!
         * @param size - size of data in BYTES
         */
        void InitializeOnGpu(const void* data, const size_t size);

        /**
         * @brief Allocates a new buffer with the given data and puts it on the CPU. The buffer will be visible both to
         * the device (GPU) and host (CPU)
         * @param data - Pointer to a block of data to allocate on the buffer. Note that the data is being copied!
         * @param size - size of data in BYTES
         * @param isMappable - Allows the buffer to return a pointer to the buffer, making it able to transfer data into.
         * The pointer to the buffer is located in m_AllocationInfo.pMappedData.
         */
        void InitializeOnCpu(const void* data, const size_t size, const bool isMappable);

        vk::BufferUsageFlags GetUsageFlags() const;
        uint32_t GetSize() const;
        vk::Buffer GetVkBuffer() const;
        VmaAllocation GetVmaAllocation() const;
        VmaAllocationInfo GetVmaAllocationInfo() const;

        void SetVkBuffer(const VkBuffer& buffer);
        void SetVmaAllocation(const VmaAllocation& allocation);
        void SetVmaAllocationInfo(const VmaAllocationInfo& allocationInfo);
        void SetUsageFlags(const vk::BufferUsageFlags& flags);

        /**
         * @brief Destroys the Vulkan buffer and frees its memory.
         */
        void Destroy();

      private:
        size_t m_Size = 0;

        vk::BufferUsageFlags m_UsageFlags;
        VkBuffer m_Buffer = VK_NULL_HANDLE;

        VmaAllocation m_Allocation = {};
        VmaAllocationInfo m_AllocationInfo = {};
    };
} // namespace VkCore
