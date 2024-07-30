#pragma once

#include "Mesh/MeshVertex.h"
#include "Model/Structures/AABB.h"
#include "Vk/Buffers/Buffer.h"
#include <cstdint>
#include <vector>

struct LODMeshInfo
{
    uint32_t lodMeshletCount[8] = {
        0, 0, 0, 0, 0, 0, 0, 0,
    };
    uint32_t lodMeshletOffsets[8] = {
        0, 0, 0, 0, 0, 0, 0, 0,
    };
    alignas(16) uint32_t LodCount = 0;
};

struct LODData;
class LODMesh
{
  public:
    LODMesh(const std::vector<LODData>& lodData);

    vk::DescriptorSet GetDescriptorSet() const
    {
        return m_DescriptorSet;
    }
    vk::DescriptorSetLayout GetDescriptorSetLayout() const
    {
        return m_DescriptorSetLayout;
    }
    LODMeshInfo GetMeshInfo() const
    {
        return m_LodInfo;
    }

    void Destroy()
    {
        m_VertexBuffer.Destroy();
        m_MeshletVerticesBuffer.Destroy();
        m_MeshletVerticesBuffer.Destroy();
        m_MeshletBuffer.Destroy();
        m_MeshletBoundsBuffer.Destroy();
        m_LodBuffer.Destroy();
    }

    static AABB CreateBoundingBox(const LODMesh& mesh);

    std::vector<MeshVertex> vertices;

  private:
    LODMeshInfo m_LodInfo;

    VkCore::Buffer m_VertexBuffer;
    VkCore::Buffer m_MeshletVerticesBuffer;
    VkCore::Buffer m_MeshletTrianglesBuffer;
    VkCore::Buffer m_MeshletBuffer;
    VkCore::Buffer m_MeshletBoundsBuffer;
    VkCore::Buffer m_LodBuffer;

    vk::DescriptorSet m_DescriptorSet;
    vk::DescriptorSetLayout m_DescriptorSetLayout;
};
