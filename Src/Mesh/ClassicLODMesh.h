
#pragma once

#include "Mesh/MeshVertex.h"
#include "Model/Structures/AABB.h"
#include "Vk/Buffers/Buffer.h"
#include "vulkan/vulkan_handles.hpp"
#include <cstdint>
#include <vector>

struct ClassicLODMeshInfo
{
    uint32_t indexCount[8] = {
        0, 0, 0, 0, 0, 0, 0, 0,
    };

    uint32_t indexOffset[8] = {
        0, 0, 0, 0, 0, 0, 0, 0,
    };

    uint32_t vertexCount[8] = {
        0, 0, 0, 0, 0, 0, 0, 0,
    };

     uint32_t LodCount = 0;
};

struct LODData;
class ClassicLODMesh
{
  public:
    ClassicLODMesh(const std::vector<LODData>& lodData);

    ClassicLODMeshInfo GetMeshInfo() const
    {
        return m_LodInfo;
    }

    VkCore::Buffer& GetVertexBuffer()
    {
        return m_VertexBuffer;
    }

    VkCore::Buffer& GetIndexBuffer()
    {
        return m_IndexBuffer;
    }

	void BindVertexBuffer(const vk::CommandBuffer& cmdBuffer) {
		cmdBuffer.bindVertexBuffers(0, m_VertexBuffer.GetVkBuffer(), {0});
	}

    void Destroy()
    {
        m_VertexBuffer.Destroy();
        m_IndexBuffer.Destroy();
    }

    static AABB CreateBoundingBox(const ClassicLODMesh& mesh);

    std::vector<Vertex> vertices;

  private:
    ClassicLODMeshInfo m_LodInfo;

    VkCore::Buffer m_VertexBuffer;
    VkCore::Buffer m_IndexBuffer;
};
