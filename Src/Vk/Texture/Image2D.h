#pragma once

#include "../Buffers/Buffer.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_handles.hpp"
#include "vulkan/vulkan_structs.hpp"
#include <cstdint>

namespace VkCore
{
    /**
     * This class is used for compute pipelines only!
     */
    class Image2D
    {
      public:
        Image2D() {};
		
		void Destroy();

        void InitializeOnTheGpu(const uint32_t width, const uint32_t height, const vk::Format format);
        void TransitionToGeneral(const vk::CommandBuffer& cmdBuffer,
                                 const vk::PipelineStageFlags srcStageMask = vk::PipelineStageFlagBits::eTopOfPipe,
                                 const vk::PipelineStageFlags dstStageMask = vk::PipelineStageFlagBits::eComputeShader);

        void TransitionToShaderRead(const vk::CommandBuffer& cmdBuffer, const vk::PipelineStageFlags srcStageMask,
                                    const vk::PipelineStageFlags dstStageMask);

		vk::DescriptorImageInfo CreateDescriptorImageInfo(const vk::ImageLayout imageLayout);
		
        vk::Image GetImage() const
        {
            return m_Image;
        }

        vk::Sampler GetSampler() const
        {
            return m_Sampler;
        }

        vk::ImageView GetView() const
        {
			return m_ImageView;
        }

      private:
        vk::Sampler m_Sampler;
        vk::Image m_Image;
        vk::ImageView m_ImageView;
        vk::Format m_Format;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        VmaAllocation m_Allocation;
        VmaAllocationInfo m_AllocationInfo;
    };

} // namespace VkCore
