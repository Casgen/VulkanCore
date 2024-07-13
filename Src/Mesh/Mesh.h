#pragma once

#include <cstdint>
#include <vector>

#include "../Vk/Buffers/Buffer.h"
#include "Mesh/Meshlet.h"
#include "MeshVertex.h"
#include "Model/Structures/OcTree.h"

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

    void Destroy()
    {
        m_VertexBuffer.Destroy();
        m_MeshletVerticesBuffer.Destroy();
        m_MeshletVerticesBuffer.Destroy();
        m_MeshletBuffer.Destroy();
        m_MeshletBoundsBuffer.Destroy();
    }

    static OcTreeTriangles OcTreeMesh(const Mesh& mesh, const uint32_t capacity);
    static AABB CreateBoundingBox(const Mesh& mesh);

    std::vector<uint32_t> indices;
    const std::vector<MeshVertex> vertices;

  private:
    uint32_t m_MeshletCount = 0;

    VkCore::Buffer m_VertexBuffer;
    VkCore::Buffer m_MeshletVerticesBuffer;
    VkCore::Buffer m_MeshletTrianglesBuffer;
    VkCore::Buffer m_MeshletBuffer;
    VkCore::Buffer m_MeshletBoundsBuffer;

    vk::DescriptorSet m_DescriptorSet;
    vk::DescriptorSetLayout m_DescriptorSetLayout;
};
