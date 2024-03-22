#include "DepthImage.h"
#include "../Utils.h"
#include "../Services/ServiceLocator.h"
#include "../Devices/DeviceManager.h"
#include "vulkan/vulkan.hpp"
#include "../../Log/Log.h"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_structs.hpp"

namespace VkCore
{
    DepthImage::DepthImage(const uint32_t width, const uint32_t height)
    {

        m_Format = VkCore::Utils::FindSupportedFormat(
            {vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint, vk::Format::eD32Sfloat},
            vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);

        VmaAllocationCreateInfo allocCreateInfo{};
        allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

        TRY_CATCH_BEGIN()

        m_Image = ServiceLocator::GetAllocatorService().CreateImage(width, height, m_Format, vk::ImageTiling::eOptimal,
                                                                    vk::ImageUsageFlagBits::eDepthStencilAttachment,
                                                                    allocCreateInfo, m_Allocation, &m_AllocationInfo);

        TRY_CATCH_END()

        vk::ImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.image = m_Image;
        imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
        imageViewCreateInfo.format = m_Format;
        imageViewCreateInfo.components.a = vk::ComponentSwizzle::eIdentity;
        imageViewCreateInfo.components.r = vk::ComponentSwizzle::eIdentity;
        imageViewCreateInfo.components.g = vk::ComponentSwizzle::eIdentity;
        imageViewCreateInfo.components.b = vk::ComponentSwizzle::eIdentity;
        imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;

        TRY_CATCH_BEGIN()

        m_ImageView = DeviceManager::GetDevice().CreateImageView(imageViewCreateInfo);

        TRY_CATCH_END()
    }

    void DepthImage::Destroy()
    {
        DeviceManager::GetDevice().DestroyImageView(m_ImageView);
        ServiceLocator::GetAllocatorService().DestroyImage(m_Image, m_Allocation);
    }

} // namespace VkCore
