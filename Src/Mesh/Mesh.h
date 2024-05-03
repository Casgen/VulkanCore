#pragma once

#include <cstdint>
#include <vector>

#include "../Vk/Buffers/Buffer.h"
#include "MeshVertex.h"

class Mesh
{
  public:
    Mesh(const std::vector<uint32_t>& indices, const std::vector<MeshVertex>& vertices);

    vk::DescriptorSet GetDescriptorSet() const
    {
        return m_DescriptorSet;
    }
    vk::DescriptorSetLayout GetDescriptorSetLayout() const
    {
        return m_DescriptorSetLayout;
    }
    uint32_t GetMeshletCount() const
    {
        return m_MeshletCount;
    }

  private:
    const std::vector<uint32_t> m_Indices;
    const std::vector<MeshVertex> m_Vertices;
    uint32_t m_MeshletCount = 0;

    VkCore::Buffer m_VertexBuffer;
    VkCore::Buffer m_MeshletBuffer;

    vk::DescriptorSet m_DescriptorSet;
    vk::DescriptorSetLayout m_DescriptorSetLayout;
};
