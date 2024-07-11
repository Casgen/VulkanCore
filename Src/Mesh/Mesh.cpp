#include "Mesh.h"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <immintrin.h>
#include <limits>

#include "../Constants.h"
#include "../Log/Log.h"
#include "../Vk/Buffers/Buffer.h"
#include "../Vk/Descriptors/DescriptorBuilder.h"
#include "../Vk/Devices/DeviceManager.h"
#include "Mesh/Meshlet.h"
#include "Mesh/MeshletGeneration.h"
#include "Mesh/MeshUtils.h"
#include "Meshlet.h"
#include "vulkan/vulkan_enums.hpp"

Mesh::Mesh(const std::vector<uint32_t>& indices, const std::vector<MeshVertex>& vertices)
    : indices(indices), vertices(vertices)
{

    m_VertexBuffer = VkCore::Buffer(vk::BufferUsageFlagBits::eStorageBuffer);
    m_VertexBuffer.InitializeOnGpu(vertices.data(), vertices.size() * sizeof(MeshVertex));

    std::vector<uint32_t> meshletVertices;
    std::vector<uint32_t> meshletTriangles;

    std::vector<uint32_t> tipsifiedIndices = MeshUtils::Tipsify(indices, vertices.size(), 24);

    std::vector<NewMeshlet> meshlets =
        MeshletGeneration::MeshletizeNv(Constants::MAX_MESHLET_VERTICES, Constants::MAX_MESHLET_INDICES, tipsifiedIndices,
                                        vertices.size(), meshletVertices, meshletTriangles);

	LOGF(Rendering, Verbose, "Number of meshlets: %d", meshlets.size())

    meshletVertices.shrink_to_fit();
    meshletTriangles.shrink_to_fit();

    m_MeshletCount = meshlets.size();

    m_MeshletVerticesBuffer = VkCore::Buffer(vk::BufferUsageFlagBits::eStorageBuffer);
    m_MeshletVerticesBuffer.InitializeOnGpu(meshletVertices.data(), meshletVertices.size() * sizeof(uint32_t));

	std::vector<MeshletBounds> meshletBounds = MeshletGeneration::ComputeMeshletBounds(vertices, meshletVertices, meshlets);

    m_MeshletBoundsBuffer = VkCore::Buffer(vk::BufferUsageFlagBits::eStorageBuffer);
    m_MeshletBoundsBuffer.InitializeOnGpu(meshletBounds.data(), meshletBounds.size() * sizeof(MeshletBounds));



    m_MeshletTrianglesBuffer = VkCore::Buffer(vk::BufferUsageFlagBits::eStorageBuffer);
    m_MeshletTrianglesBuffer.InitializeOnGpu(meshletTriangles.data(), meshletTriangles.size() * sizeof(uint32_t));

    m_MeshletBuffer = VkCore::Buffer(vk::BufferUsageFlagBits::eStorageBuffer);
    m_MeshletBuffer.InitializeOnGpu(meshlets.data(), meshlets.size() * sizeof(NewMeshlet));

    VkCore::DescriptorBuilder descBuilder = VkCore::DescriptorBuilder(VkCore::DeviceManager::GetDevice());

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
                                   vk::ShaderStageFlagBits::eMeshNV | vk::ShaderStageFlagBits::eTaskEXT)
                       .Build(m_DescriptorSet, m_DescriptorSetLayout);

    ASSERT(success, "Failed to build a descriptor set for a mesh!")
}
OcTreeTriangles Mesh::OcTreeMesh(const Mesh& mesh, const uint32_t capacity)
{

    std::vector<IndexedTriangle> triangles;
    triangles.reserve(mesh.indices.size() / 3);

    if (mesh.indices.size() % 3 != 0)
    {
        LOGF(Rendering, Warning,
             "The number of indices in the model do not look like triangles! Mesh may not be complete! Indices count: "
             "%d",
             mesh.indices.size())
    }

    for (size_t i = 0; i < mesh.indices.size() - 3; i += 3)
    {
        triangles.emplace_back(mesh.vertices[mesh.indices[i]].Position, mesh.vertices[mesh.indices[i + 1]].Position,
                               mesh.vertices[mesh.indices[i + 2]].Position, mesh.indices[i], mesh.indices[i + 1],
                               mesh.indices[i + 2]);
    }

    OcTreeTriangles ocTree = OcTreeTriangles(Mesh::CreateBoundingBox(mesh), capacity);

    for (const auto& triangle : triangles)
    {
        ocTree.Push(triangle);
    }

    return ocTree;
}

AABB Mesh::CreateBoundingBox(const Mesh& mesh)
{

    __m256 maxPoint = _mm256_set_ps(0.f, 0.f, std::numeric_limits<float>::min(), std::numeric_limits<float>::min(),
                                    std::numeric_limits<float>::min(), std::numeric_limits<float>::min(),
                                    std::numeric_limits<float>::min(), std::numeric_limits<float>::min());

    __m256 minPoint = _mm256_set_ps(0.f, 0.f, std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
                                    std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
                                    std::numeric_limits<float>::max(), std::numeric_limits<float>::max());

    size_t i = 0;

    while (i < (mesh.vertices.size()) - 2)
    {
        // clang-format off
        __m256 temp = _mm256_set_ps(0.f,
									0.f,
									mesh.vertices[i + 1].Position.z,
									mesh.vertices[i + 1].Position.y,
                                    mesh.vertices[i + 1].Position.x,
									mesh.vertices[i].Position.z,
                                    mesh.vertices[i].Position.y,
									mesh.vertices[i].Position.x);
        // clang-format on

        maxPoint = _mm256_max_ps(maxPoint, temp);
        minPoint = _mm256_min_ps(minPoint, temp);

        i += 2;
    }

    if ((mesh.vertices.size() - i) == 1)
    {
        // clang-format off
        __m256 temp = _mm256_set_ps(0.f, 
									0.f,
									mesh.vertices[i].Position.z,
									mesh.vertices[i].Position.y,
                                    mesh.vertices[i].Position.x,
									mesh.vertices[i].Position.z,
                                    mesh.vertices[i].Position.y,
									mesh.vertices[i].Position.x);
        // clang-format on

        maxPoint = _mm256_max_ps(maxPoint, temp);
        minPoint = _mm256_min_ps(minPoint, temp);

        LOG(Rendering, Verbose, "Found 1 remaining triangles to meshletize")
    }

    __m128 finalMaxPoint = _mm_max_ps(_mm_set_ps(0.f, maxPoint[2], maxPoint[1], maxPoint[0]),
                                      _mm_set_ps(0.f, maxPoint[5], maxPoint[4], maxPoint[3]));
    __m128 finalMinPoint = _mm_max_ps(_mm_set_ps(0.f, minPoint[2], minPoint[1], minPoint[0]),
                                      _mm_set_ps(0.f, minPoint[5], minPoint[4], minPoint[3]));

    return {
        .minPoint = {finalMinPoint[0], finalMinPoint[1], finalMinPoint[2]},
        .maxPoint = {finalMaxPoint[0], finalMaxPoint[1], finalMaxPoint[2]},
    };
}
