#include "LODMesh.h"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <immintrin.h>

#include "../Constants.h"
#include "../Log/Log.h"
#include "../Vk/Buffers/Buffer.h"
#include "../Vk/Descriptors/DescriptorBuilder.h"
#include "../Vk/Devices/DeviceManager.h"
#include "Mesh/LODModel.h"
#include "Mesh/Meshlet.h"
#include "Mesh/MeshletGeneration.h"
#include "Mesh/MeshUtils.h"
#include "Meshlet.h"
#include "vulkan/vulkan_enums.hpp"

LODMesh::LODMesh(const std::vector<LODData>& lodData)
{
    ASSERT(lodData.size() <= 8, "There are more LODs than supported");

    m_LodInfo.LodCount = lodData.size();

    std::vector<NewMeshlet> allMeshlets(0);
    std::vector<uint32_t> allMeshletVertices(0);
    std::vector<uint32_t> allMeshletTriangles(0);

    // Accumulating vertex offset replaced in the meshlets. Offsets into the allmeshletVertices.
    uint32_t accVerticesOffset = 0;
    // Accumulating index offset replaced in the meshlets. Offsets into the allmeshletTriangle.
    uint32_t accTriangleOffset = 0;

    uint32_t accLodMeshletOffset = 0;

    for (uint8_t i = 0; i < lodData.size(); i++)
    {

        std::vector<uint32_t> meshletVertices;
        std::vector<uint32_t> meshletTriangles;

        std::vector<uint32_t> tipsifiedIndices = MeshUtils::Tipsify(lodData[i].indices, lodData[i].vertices.size(), 32);

        std::vector<NewMeshlet> meshlets = MeshletGeneration::MeshletizeNv(
            Constants::MAX_MESHLET_VERTICES, Constants::MAX_MESHLET_INDICES, tipsifiedIndices,
            lodData[i].vertices.size(), meshletVertices, meshletTriangles, accVerticesOffset, accTriangleOffset);

        for (uint32_t v = 0; v < meshletVertices.size(); v++)
        {
			allMeshletVertices.emplace_back(meshletVertices[v] += vertices.size());
        }

        m_LodInfo.lodMeshletCount[i] = meshlets.size();
        m_LodInfo.lodMeshletOffsets[i] = accLodMeshletOffset;

		for (uint32_t m = 0; m < meshlets.size(); m++) {
			allMeshlets.emplace_back(meshlets[m]);
		}

		for (uint32_t t = 0; t < meshletTriangles.size(); t++) {
			allMeshletTriangles.emplace_back(meshletTriangles[t]);
		}

        accLodMeshletOffset += meshlets.size();
        accVerticesOffset += meshletVertices.size();
        accTriangleOffset += meshletTriangles.size();

		for (uint32_t v = 0; v < lodData[i].vertices.size(); v++) {
			vertices.emplace_back(lodData[i].vertices[v]);
		}
    }

    std::vector<MeshletBounds> meshletBounds =
        MeshletGeneration::ComputeMeshletBounds(vertices, allMeshletVertices, allMeshlets);

    ASSERT(meshletBounds.size() == allMeshlets.size(),
           "Number of meshlet bounds doesn't match with the meshlets count!")

    m_VertexBuffer = VkCore::Buffer(vk::BufferUsageFlagBits::eStorageBuffer);
    m_VertexBuffer.InitializeOnGpu(vertices.data(), vertices.size() * sizeof(MeshVertex));

    m_MeshletVerticesBuffer = VkCore::Buffer(vk::BufferUsageFlagBits::eStorageBuffer);
    m_MeshletVerticesBuffer.InitializeOnGpu(allMeshletVertices.data(), allMeshletVertices.size() * sizeof(uint32_t));

    m_MeshletBoundsBuffer = VkCore::Buffer(vk::BufferUsageFlagBits::eStorageBuffer);
    m_MeshletBoundsBuffer.InitializeOnGpu(meshletBounds.data(), meshletBounds.size() * sizeof(MeshletBounds));

    m_MeshletTrianglesBuffer = VkCore::Buffer(vk::BufferUsageFlagBits::eStorageBuffer);
    m_MeshletTrianglesBuffer.InitializeOnGpu(allMeshletTriangles.data(), allMeshletTriangles.size() * sizeof(uint32_t));

    m_MeshletBuffer = VkCore::Buffer(vk::BufferUsageFlagBits::eStorageBuffer);
    m_MeshletBuffer.InitializeOnGpu(allMeshlets.data(), allMeshlets.size() * sizeof(NewMeshlet));

    VkCore::DescriptorBuilder descBuilder = VkCore::DescriptorBuilder(VkCore::DeviceManager::GetDevice());

    m_LodBuffer = VkCore::Buffer(vk::BufferUsageFlagBits::eStorageBuffer);
    m_LodBuffer.InitializeOnGpu(&m_LodInfo, sizeof(LODMeshInfo));

    bool success = descBuilder
                       .BindBuffer(0, m_VertexBuffer, vk::DescriptorType::eStorageBuffer,
                                   vk::ShaderStageFlagBits::eMeshNV | vk::ShaderStageFlagBits::eTaskEXT)
                       .BindBuffer(1, m_MeshletBuffer, vk::DescriptorType::eStorageBuffer,
                                   vk::ShaderStageFlagBits::eMeshNV | vk::ShaderStageFlagBits::eTaskEXT)
                       .BindBuffer(2, m_MeshletVerticesBuffer, vk::DescriptorType::eStorageBuffer,
                                   vk::ShaderStageFlagBits::eMeshNV | vk::ShaderStageFlagBits::eTaskEXT)
                       .BindBuffer(3, m_MeshletTrianglesBuffer, vk::DescriptorType::eStorageBuffer,
                                   vk::ShaderStageFlagBits::eMeshNV | vk::ShaderStageFlagBits::eTaskEXT)
                       .BindBuffer(4, m_MeshletBoundsBuffer, vk::DescriptorType::eStorageBuffer,
                                   vk::ShaderStageFlagBits::eMeshNV | vk::ShaderStageFlagBits::eTaskEXT |
                                       vk::ShaderStageFlagBits::eVertex)
                       .BindBuffer(5, m_LodBuffer, vk::DescriptorType::eStorageBuffer,
                                   vk::ShaderStageFlagBits::eMeshNV | vk::ShaderStageFlagBits::eTaskEXT |
                                       vk::ShaderStageFlagBits::eVertex)
                       .Build(m_DescriptorSet, m_DescriptorSetLayout);

    ASSERT(success, "Failed to build a descriptor set for a mesh!")
}
