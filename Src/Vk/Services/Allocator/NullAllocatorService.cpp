#include "NullAllocatorService.h"
#include "../../../Log/Log.h"
#include "vulkan/vulkan_core.h"

namespace VkCore
{

    VkBuffer NullAllocatorService::CreateBuffer(const size_t size, const std::vector<uint32_t> queueFamilyIndices,
                                                const vk::BufferUsageFlags usageFlags,
                                                const vk::BufferCreateFlags createFlags,
                                                const VmaMemoryUsage memoryUsage,
                                                const VmaAllocationCreateFlags allocFlags, VmaAllocation& outAllocation,
                                                VmaAllocationInfo* outAllocationInfo)
    {
        LOG(Allocation, Fatal,
            "Allocation service couldn't be located! Please make sure you have provided an allocation service!")

        return VK_NULL_HANDLE;
    }

    VkImage NullAllocatorService::CreateImage(const void* data, const VkDeviceSize size,
                                              const vk::ImageCreateInfo& createInfo,
                                              const VmaAllocationCreateInfo& allocCreateInfo,
                                              VmaAllocation& outAllocation, VmaAllocationInfo* outAllocationInfo)
    {
        LOG(Allocation, Fatal,
            "Allocation service couldn't be located! Please make sure you have provided an allocation service!")

        return VK_NULL_HANDLE;
    }

    VkImage NullAllocatorService::CreateImage(const VkDeviceSize size, const vk::ImageCreateInfo& createInfo,
                                              const VmaAllocationCreateInfo& allocCreateInfo,
                                              VmaAllocation& outAllocation, VmaAllocationInfo* outAllocationInfo)
    {
        LOG(Allocation, Fatal,
            "Allocation service couldn't be located! Please make sure you have provided an allocation service!")

        return VK_NULL_HANDLE;
    }
    VkImage NullAllocatorService::CreateImage(const uint32_t width, const uint32_t height, const vk::Format format,
                                              vk::ImageTiling imageTiling, vk::ImageUsageFlags usageFlags,
                                              const VmaAllocationCreateInfo& allocCreateInfo,
                                              VmaAllocation& outAllocation, VmaAllocationInfo* outAllocationInfo)
    {
        LOG(Allocation, Fatal,
            "Allocation service couldn't be located! Please make sure you have provided an allocation service!")

        return VK_NULL_HANDLE;
    }

    void NullAllocatorService::DestroyBuffer(Buffer& buffer)
    {
        LOG(Allocation, Fatal,
            "Allocation service couldn't be located! Please make sure you have provided an allocation service!")
    }

    void NullAllocatorService::DestroyImage(vk::Image& image, VmaAllocation& allocation)
    {
        LOG(Allocation, Fatal,
            "Allocation service couldn't be located! Please make sure you have provided an allocation service!")
    }

    void NullAllocatorService::CopyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, const size_t size,
                                          const uint32_t srcOffset, const uint32_t dstOffset)
    {
        LOG(Allocation, Fatal,
            "Allocation service couldn't be located! Please make sure you have provided an allocation service!")
    }

    void NullAllocatorService::CopyBufferToImage(const VkImage& image, const VkBuffer& srcBuffer,
                                                 const VkDeviceSize size, const vk::Extent2D& resolution){

        LOG(Allocation, Fatal,
            "Allocation service couldn't be located! Please make sure you have provided an allocation service!")}

    VkBuffer NullAllocatorService::CreateBufferOnGpu(const void* data, const size_t size,
                                                     const vk::BufferUsageFlags usageFlags, VmaAllocation& allocation,
                                                     VmaAllocationInfo* allocationInfo)
    {
        LOG(Allocation, Fatal,
            "Allocation service couldn't be located! Please make sure you have provided an allocation service!")

        return VK_NULL_HANDLE;
    }
    void NullAllocatorService::UpdateBufferOnGpu(const Buffer& buffer, const void* data, size_t size)
    {

        LOG(Allocation, Fatal,
            "Allocation service couldn't be located! Please make sure you have provided an allocation service!")
    }

    /**
     * @brief Maps the buffer memory and returns back a pointer to the VkBuffer memory. It can be used for updating the
     * data
     */
    void NullAllocatorService::MapMemory(const VmaAllocation& allocation, void* mappedPtr)
    {
        LOG(Allocation, Fatal,
            "Allocation service couldn't be located! Please make sure you have provided an allocation service!")
    }

    /**
     * @brief unmaps the buffer memory, making the mapped pointer invalid.
     */
    void NullAllocatorService::UnmapMemory(const VmaAllocation& allocation)
    {
        LOG(Allocation, Fatal,
            "Allocation service couldn't be located! Please make sure you have provided an allocation service!")
    }

} // namespace VkCore
