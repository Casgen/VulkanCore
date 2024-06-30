#include "Mesh.h"

#include <immintrin.h>
#include <stdexcept>

#include "../Constants.h"
#include "../Log/Log.h"
#include "../Vk/Buffers/Buffer.h"
#include "../Vk/Descriptors/DescriptorBuilder.h"
#include "../Vk/Devices/DeviceManager.h"
#include "Meshlet.h"
#include "MeshletGeneration.h"
#include "Model/Structures/OcTree.h"
#include "vulkan/vulkan_enums.hpp"

Mesh::Mesh(const std::vector<uint32_t>& indices, const std::vector<MeshVertex>& vertices)
    : indices(indices), vertices(vertices)
{

    m_VertexBuffer = VkCore::Buffer(vk::BufferUsageFlagBits::eStorageBuffer);
    m_VertexBuffer.InitializeOnGpu(vertices.data(), vertices.size() * sizeof(MeshVertex));

    const std::vector<Meshlet> meshlets = MeshletGeneration::OcTreeMeshletizeMesh(
        Constants::MAX_MESHLET_VERTICES, Constants::MAX_MESHLET_INDICES, *this);

    m_MeshletCount = meshlets.size();

    m_MeshletBuffer = VkCore::Buffer(vk::BufferUsageFlagBits::eStorageBuffer);
    m_MeshletBuffer.InitializeOnGpu(meshlets.data(), meshlets.size() * sizeof(Meshlet));

    VkCore::DescriptorBuilder descBuilder = VkCore::DescriptorBuilder(VkCore::DeviceManager::GetDevice());

    bool success =
        descBuilder.BindBuffer(0, m_VertexBuffer, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eMeshNV)
            .BindBuffer(1, m_MeshletBuffer, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eMeshNV)
            .Build(m_DescriptorSet, m_DescriptorSetLayout);

    if (!success)
    {
        LOG(Vulkan, Fatal, "Failed to build a descriptor set for a mesh!")
        throw std::runtime_error("Failed to build a descriptor set for a mesh!");
    }
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

	for (size_t i = 0; i < mesh.indices.size() - 3; i += 3) {
        triangles.emplace_back(mesh.vertices[mesh.indices[i]].Position, mesh.vertices[mesh.indices[i + 1]].Position,
                               mesh.vertices[mesh.indices[i + 2]].Position, mesh.indices[i], mesh.indices[i + 1],
                               mesh.indices[i + 2]);
	}

    OcTreeTriangles ocTree = OcTreeTriangles(Mesh::CreateBoundingBox(mesh), capacity);

	for (const auto& triangle : triangles) {
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
