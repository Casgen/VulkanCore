#pragma once

#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_handles.hpp"
#include "vk_mem_alloc.h"

namespace VkCore
{
    class DepthImage
    {

      public:
        DepthImage(){};
        DepthImage(const uint32_t width, const uint32_t height);
        void Destroy();

        bool HasStencilComponent() const
        {
            return m_Format == vk::Format::eD32SfloatS8Uint || m_Format == vk::Format::eD24UnormS8Uint;
        }

        vk::Format GetFormat() const
        {
            return m_Format;
        }

        vk::ImageView& GetImageView()
        {
            return m_ImageView;
        }

      private:
        vk::Image m_Image = VK_NULL_HANDLE;
        vk::ImageView m_ImageView = VK_NULL_HANDLE;
        vk::Format m_Format;

        VmaAllocation m_Allocation = VK_NULL_HANDLE;
        VmaAllocationInfo m_AllocationInfo;
    };

} // namespace VkCore
