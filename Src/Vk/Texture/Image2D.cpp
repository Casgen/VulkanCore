
#include "Image2D.h"
#include "../Services/ServiceLocator.h"
#include "Vk/Buffers/Buffer.h"
#include "Vk/Devices/Device.h"
#include "Vk/Devices/DeviceManager.h"
#include "Vk/Services/Allocator/IAllocatorService.h"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_handles.hpp"
#include "vulkan/vulkan_structs.hpp"
#include "../../Log/Log.h"

namespace VkCore
{

    void Image2D::Destroy()
    {
		VkCore::Device& device = VkCore::DeviceManager::GetDevice();

		device.DestroySampler(m_Sampler);
		device.DestroyImageView(m_ImageView);

		ServiceLocator::GetAllocatorService().DestroyImage(m_Image, m_Allocation);
    }

    void Image2D::InitializeOnTheGpu(const uint32_t width, const uint32_t height, const vk::Format format)
    {

        m_Width = width;
        m_Height = height;

        VmaAllocation stagingAllocation;
        VmaAllocationInfo stagingAllocationInfo;

        IAllocatorService& allocService = ServiceLocator::GetAllocatorService();

        vk::ImageCreateInfo imageCreateInfo{};
        imageCreateInfo.setImageType(vk::ImageType::e2D)
            .setExtent({width, height, 1})
            .setMipLevels(1)
            .setArrayLayers(1)
            .setTiling(vk::ImageTiling::eOptimal)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setUsage(vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eSampled)
            .setSharingMode(vk::SharingMode::eExclusive)
            .setSamples(vk::SampleCountFlagBits::e1)
            .setFormat(format);

        uint32_t formatInBytes = 0;

        switch (format)
        {
        case vk::Format::eR32G32B32A32Sfloat:
            formatInBytes = 16;
            break;
        case vk::Format::eR32G32B32A32Sint:
            formatInBytes = 16;
            break;
        case vk::Format::eR32Sfloat:
            formatInBytes = 4;
            break;
        case vk::Format::eR8Unorm:
            formatInBytes = 1;
            break;
        default:
            ASSERT(formatInBytes != 0, "Couldn't find the appropriate format!")
        }

        // TODO: Determine format size in bytes.

        VmaAllocationCreateInfo allocCreateInfo{};
        allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        allocCreateInfo.pool = nullptr;

        m_Image = ServiceLocator::GetAllocatorService().CreateImage(formatInBytes * width * height, imageCreateInfo,
                                                                    allocCreateInfo, m_Allocation, &m_AllocationInfo);

        VkCore::Device& device = VkCore::DeviceManager::GetDevice();

        vk::CommandPool cmdPool;
        vk::CommandBuffer cmdBuffer = device.BeginSingleTimeCommands(cmdPool);

        vk::ImageMemoryBarrier memoryBarrier{};
        memoryBarrier.oldLayout = vk::ImageLayout::eUndefined;
        memoryBarrier.newLayout = vk::ImageLayout::eGeneral;
        memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        memoryBarrier.image = m_Image;
        memoryBarrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        memoryBarrier.subresourceRange.baseMipLevel = 0;
        memoryBarrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
        memoryBarrier.subresourceRange.baseArrayLayer = 0;
        memoryBarrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
        memoryBarrier.srcAccessMask = vk::AccessFlagBits::eNone;
        memoryBarrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;

        cmdBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eComputeShader, {}, {},
                                  {}, memoryBarrier);

        device.EndSingleTimeCommands(cmdBuffer, cmdPool);

        vk::ImageViewCreateInfo viewCreateInfo{};
        viewCreateInfo.format = format;
        viewCreateInfo.image = m_Image;
        viewCreateInfo.viewType = vk::ImageViewType::e2D;
        viewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        viewCreateInfo.subresourceRange.baseMipLevel = 0;
        viewCreateInfo.subresourceRange.levelCount = 1;
        viewCreateInfo.subresourceRange.baseArrayLayer = 0;
        viewCreateInfo.subresourceRange.layerCount = 1;

        m_ImageView = device.CreateImageView(viewCreateInfo);

        vk::SamplerCreateInfo samplerCreateInfo{};

        samplerCreateInfo.magFilter = vk::Filter::eNearest;
        samplerCreateInfo.minFilter = vk::Filter::eNearest;
        samplerCreateInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
        samplerCreateInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
        samplerCreateInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
        samplerCreateInfo.anisotropyEnable = false;
        samplerCreateInfo.maxAnisotropy = 0;
        samplerCreateInfo.borderColor = vk::BorderColor::eFloatOpaqueBlack;
        samplerCreateInfo.unnormalizedCoordinates = false;
        samplerCreateInfo.compareEnable = false;
        samplerCreateInfo.compareOp = vk::CompareOp::eAlways;
        samplerCreateInfo.mipmapMode = vk::SamplerMipmapMode::eNearest;
        samplerCreateInfo.mipLodBias = 0.f;
        samplerCreateInfo.minLod = 0.f;
        samplerCreateInfo.maxLod = 0.f;

        m_Sampler = device.CreateSampler(samplerCreateInfo);
    }

    vk::DescriptorImageInfo Image2D::CreateDescriptorImageInfo(const vk::ImageLayout imageLayout)
    {
        vk::DescriptorImageInfo imageInfo;

        imageInfo.sampler = m_Sampler;
        imageInfo.imageView = m_ImageView;
        imageInfo.imageLayout = imageLayout;

        return imageInfo;
    }

    void Image2D::TransitionToGeneral(const vk::CommandBuffer& cmdBuffer, const vk::PipelineStageFlags srcStageMask,
                                      const vk::PipelineStageFlags dstStageMask)
    {

        vk::ImageMemoryBarrier memoryBarrier{};
        memoryBarrier.oldLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        memoryBarrier.newLayout = vk::ImageLayout::eGeneral;
        memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        memoryBarrier.image = m_Image;
        memoryBarrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        memoryBarrier.subresourceRange.baseMipLevel = 0;
        memoryBarrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
        memoryBarrier.subresourceRange.baseArrayLayer = 0;
        memoryBarrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
        memoryBarrier.srcAccessMask = vk::AccessFlagBits::eShaderRead;
        memoryBarrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;

        cmdBuffer.pipelineBarrier(srcStageMask, dstStageMask, {}, {}, {}, memoryBarrier);

    }

    void Image2D::TransitionToShaderRead(const vk::CommandBuffer& cmdBuffer, const vk::PipelineStageFlags srcStageMask,
                                         const vk::PipelineStageFlags dstStageMask)
    {

        vk::ImageMemoryBarrier memoryBarrier{};
        memoryBarrier.oldLayout = vk::ImageLayout::eGeneral;
        memoryBarrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        memoryBarrier.image = m_Image;
        memoryBarrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        memoryBarrier.subresourceRange.baseMipLevel = 0;
        memoryBarrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
        memoryBarrier.subresourceRange.baseArrayLayer = 0;
        memoryBarrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
        memoryBarrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
        memoryBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        cmdBuffer.pipelineBarrier(srcStageMask, dstStageMask, {}, {}, {}, memoryBarrier);

    }
} // namespace VkCore
