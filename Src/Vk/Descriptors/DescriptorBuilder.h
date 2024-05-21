#pragma once

#include <cstdint>
#include <memory>

#include "../Devices/Device.h"
#include "../Buffers/Buffer.h"
#include "vulkan/vulkan.hpp"

namespace VkCore
{
    class DescriptorAllocator;
    class DescriptorLayoutCache;

    class DescriptorBuilder
    {
      public:
        DescriptorBuilder();
        DescriptorBuilder(DescriptorBuilder&& other);
        DescriptorBuilder& operator=(DescriptorBuilder&& other);

        /**
         *   @brief Initializes the DescriptorBuilder with the DescriptorAllocator and DescriptorLayoutCache.
         *   @brief device
         */
        DescriptorBuilder(const Device& device);

        DescriptorBuilder& BindBuffer(uint32_t binding, const vk::DescriptorBufferInfo& bufferInfo,
                                      vk::DescriptorType type, vk::ShaderStageFlags stageFlags)
        {
            // create the descriptor binding for the layout
            vk::DescriptorSetLayoutBinding newBinding(binding, type, 1, stageFlags, nullptr);
            m_Bindings.push_back(newBinding);

            vk::DescriptorBufferInfo* bufferInfoCopy = new vk::DescriptorBufferInfo(bufferInfo);

            // create the descriptor write
            vk::WriteDescriptorSet newWrite({}, binding, {}, 1, type, nullptr, bufferInfoCopy, nullptr);

            m_Writes.push_back(newWrite);
            return *this;
        }

        /**
         * @brief Creates a Descriptor Layout binding for the given buffers with the provided binding index.
         * @param binding - binding index to bound the buffers onto (Has to reflect also in the shader file)
         * @param buffer - buffer to bind
         * @param descriptorType
         * @param stageFlags - describes which shader stage should the buffer be bound to. (Where you will be using
         * it)
         */
        DescriptorBuilder& BindBuffer(uint32_t binding, const Buffer& buffer, vk::DescriptorType descriptorType,
                                      vk::ShaderStageFlags stageFlags)
        {
            vk::DescriptorBufferInfo bufferInfo{buffer.GetVkBuffer(), 0, buffer.GetSize()};
            return BindBuffer(binding, bufferInfo, descriptorType, stageFlags);
        }

        /**
         * @brief Creates a Descriptor Layout binding for the given buffers with the provided binding index. Results in
         * createing an array.
         * @param binding - binding index to bound the buffers onto (Has to reflect also in the shader file)
         * @param Buffers - buffers to bind
         * @param descriptorType
         * @param stageFlags - describes which shader stage should the buffers be bound to. (Where you will be using
         * them)
         */
        template <typename T = Buffer>
        DescriptorBuilder& BindBuffers(uint32_t binding, const std::vector<T>& buffers,
                                       vk::DescriptorType descriptorType, vk::ShaderStageFlags stageFlags)
        {
            // create the descriptor binding for the layout
            vk::DescriptorSetLayoutBinding newBinding(binding, descriptorType, 1, stageFlags, nullptr);
            m_Bindings.emplace_back(std::move(newBinding));

            vk::DescriptorBufferInfo* bufferInfos = new vk::DescriptorBufferInfo[buffers.size()];

            for (uint32_t i = 0; i < buffers.size(); i++)
            {
                bufferInfos[i] = vk::DescriptorBufferInfo{buffers[i].GetVkBuffer(), 0, buffers[i].GetSize()};
            }

            vk::WriteDescriptorSet newWrite = vk::WriteDescriptorSet();
            // TODO: Test this out with arrays of buffers
            newWrite.setDescriptorCount(3);
            newWrite.setPBufferInfo(bufferInfos);
            newWrite.setDstBinding(binding);
            newWrite.setDescriptorType(descriptorType);

            m_Writes.emplace_back(std::move(newWrite));

            return *this;
        }

        /**
         * @brief Creates a Descriptor Layout binding for the given image with the provided binding index.
         * @param binding - binding index to bound the buffers onto (Has to reflect also in the shader file)
         * @param imageInfo - image info for the descriptor
         * @param descriptorType
         * @param stageFlags - describes which shader stage should the image be bound to. (Where you will be using
         * it)
         */
        DescriptorBuilder& BindImage(uint32_t binding, const vk::DescriptorImageInfo& imageInfo,
                                     vk::DescriptorType type, vk::ShaderStageFlags stageFlags);

        bool Build(vk::DescriptorSet& set);
        bool Build(vk::DescriptorSet& set, vk::DescriptorSetLayout& layout);
        void Clear();
        void Cleanup();

      private:
        // The Descriptor builder takes ownership of the DescriptorLayoutCache's a DescriptorAllocator's memory
        // Therefore it is responsible for their deletion.

        DescriptorLayoutCache* m_Cache;
        DescriptorAllocator* m_Allocator;

        std::vector<vk::DescriptorSetLayoutBinding> m_Bindings = {};
        std::vector<vk::WriteDescriptorSet> m_Writes = {};
    };

} // namespace VkCore
