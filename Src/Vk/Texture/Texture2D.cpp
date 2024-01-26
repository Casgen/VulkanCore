
#include "Texture2D.h"
#include "../../FileUtils.h"
#include "../Services/ServiceLocator.h"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_handles.hpp"
#include "vulkan/vulkan_structs.hpp"
#include "../../Log/Log.h"

namespace VkCore
{

    Texture2D::Texture2D(const char* filename)
    {
        m_ImgData = FileUtils::ReadImage(filename);
    }

    void Texture2D::InitializeOnTheGpu(const uint32_t mipLevels)
    {
        Buffer::BufferInfo stagingBufferInfo;

        stagingBufferInfo.m_Size = m_ImgData.dataSize;
        stagingBufferInfo.m_UsageFlags = vk::BufferUsageFlagBits::eTransferSrc;
        stagingBufferInfo.m_MemoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
        stagingBufferInfo.m_AllocCreateFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT |
                                               VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                                               VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT;

        VmaAllocation stagingAllocation;
        VmaAllocationInfo stagingAllocationInfo;

        VkBuffer stagingBuffer = ServiceLocator::GetAllocatorService().CreateBuffer(
            stagingBufferInfo, stagingAllocation, &stagingAllocationInfo);

        std::memcpy(stagingAllocationInfo.pMappedData, m_ImgData.data, m_ImgData.dataSize);

        vk::ImageCreateInfo imageCreateInfo{};
        imageCreateInfo.setImageType(vk::ImageType::e2D)
            .setExtent({m_ImgData.width, m_ImgData.height, 1})
            .setMipLevels(mipLevels)
            .setArrayLayers(1)
            .setTiling(vk::ImageTiling::eOptimal)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setUsage(vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst)
            .setSharingMode(vk::SharingMode::eExclusive)
            .setSamples(vk::SampleCountFlagBits::e1);

        switch (m_ImgData.channels)
        {
        case STBI_rgb_alpha:
            imageCreateInfo.setFormat(vk::Format::eR8G8B8A8Srgb);
            break;
        case STBI_rgb:
            imageCreateInfo.setFormat(vk::Format::eR8G8B8Srgb);
            break;
        case STBI_grey:
            imageCreateInfo.setFormat(vk::Format::eR8Srgb);
            break;
        case STBI_grey_alpha:
            imageCreateInfo.setFormat(vk::Format::eR8G8Srgb);
            break;
        default: {
            LOG(Vulkan, Error, "Unrecognized image format! falling back to RGBA Srgb!")
            imageCreateInfo.setFormat(vk::Format::eR8G8B8A8Srgb);
        }
        }

        VmaAllocationCreateInfo allocCreateInfo{};
        allocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        allocCreateInfo.pool = nullptr;

        m_Image = ServiceLocator::GetAllocatorService().CreateImage(m_ImgData.data, m_ImgData.dataSize, imageCreateInfo,
                                                                    allocCreateInfo, m_Allocation, &m_AllocationInfo);

        m_ImgData.Free();
    }

    void Texture2D::TransitionImageLayout(const VkCore::Device& device, const vk::Format format,
                                          const vk::ImageLayout oldLayout, const vk::ImageLayout newLayout)
    {
        vk::CommandPool cmdPool;
        vk::CommandBuffer cmdBuffer = device.BeginSingleTimeCommands(cmdPool);

        vk::ImageMemoryBarrier memoryBarrier{};
        memoryBarrier.oldLayout = oldLayout;
        memoryBarrier.newLayout = newLayout;
        memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        memoryBarrier.image = m_Image;
        memoryBarrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        memoryBarrier.subresourceRange.baseMipLevel = 0;
        memoryBarrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
        memoryBarrier.subresourceRange.baseArrayLayer = 0;
        memoryBarrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
        memoryBarrier.srcAccessMask = vk::AccessFlagBits::eNone;
        memoryBarrier.dstAccessMask = vk::AccessFlagBits::eNone;

        cmdBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eNone, vk::PipelineStageFlagBits::eNone, {}, {}, {}, memoryBarrier);
    }
} // namespace VkCore
