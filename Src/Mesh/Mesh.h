#pragma once

#include <cstdint>
#include <vector>

#include "../Vk/Buffers/Buffer.h"
#include "MeshVertex.h"

class Mesh {
   public:
    Mesh(const std::vector<uint32_t>& indices, const std::vector<MeshVertex>& vertices);

    vk::DescriptorSet GetDescriptorSet() const { return descriptor_set_; }
    vk::DescriptorSetLayout GetDescriptorSetLayout() const { return descriptor_set_layout_; }
    uint32_t GetMeshletCount() const { return meshlet_count_; }

   private:
    const std::vector<uint32_t> indices_;
    const std::vector<MeshVertex> vertices_;
    uint32_t meshlet_count_ = 0;

    VkCore::Buffer vertex_buffer_;
    VkCore::Buffer meshlet_buffer_;

    vk::DescriptorSet descriptor_set_;
    vk::DescriptorSetLayout descriptor_set_layout_;
};
