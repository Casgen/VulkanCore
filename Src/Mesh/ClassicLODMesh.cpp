
#include "ClassicLODMesh.h"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <immintrin.h>
#include <limits>

#include "../Log/Log.h"
#include "../Vk/Buffers/Buffer.h"
#include "ClassicLODModel.h"
#include "Mesh/MeshUtils.h"
#include "glm/gtc/type_ptr.hpp"
#include "vulkan/vulkan_enums.hpp"

ClassicLODMesh::ClassicLODMesh(const std::vector<LODData>& lodData)
{
    ASSERT(lodData.size() <= 8, "There are more LODs than supported");
    ASSERT(lodData.size() > 0, "There are more no LODs to load");

    m_LodInfo.LodCount = lodData.size();

    // Accumulating vertex offset replaced.
	std::vector<Vertex> allVertices;
	std::vector<uint32_t> allIndices;

    for (uint8_t l = 0; l < lodData.size(); l++)
    {
			
		size_t vertexOffset = allVertices.size();

		std::vector<uint32_t> indices = MeshUtils::Tipsify(lodData.at(l).indices, lodData.at(l).vertices.size(), 32);

		m_LodInfo.indexCount[l] = indices.size();
		m_LodInfo.indexOffset[l] = allIndices.size();
		m_LodInfo.vertexCount[l] = lodData[l].vertices.size();

		for (uint32_t i = 0; i < indices.size(); i++) {
			allIndices.emplace_back(indices[i] + allVertices.size());
		}

		for (const auto& vertex : lodData[l].vertices) {
			allVertices.emplace_back(vertex);
		}
    }

	vertices = allVertices;

	Vec3f max = Vec3f(std::numeric_limits<float>::min());
	Vec3f min = Vec3f(std::numeric_limits<float>::max());

	for (size_t i = 0; i < m_LodInfo.vertexCount[0]; i++) {
		
		Vec3f position = Vec3f(glm::value_ptr(vertices[i].Position));

		max = Vec3f::Max(max, position);
		min = Vec3f::Min(min, position);
	}

	Vec3f sphereCenter = (max + min) / 2.0;

	m_LodInfo.sphereCenter = {sphereCenter.x, sphereCenter.y, sphereCenter.z};
	m_LodInfo.sphereRadius = (sphereCenter - max).Magnitude();

    m_VertexBuffer = VkCore::Buffer(vk::BufferUsageFlagBits::eVertexBuffer);
    m_VertexBuffer.InitializeOnGpu(vertices.data(), vertices.size() * sizeof(Vertex));

    m_IndexBuffer = VkCore::Buffer(vk::BufferUsageFlagBits::eIndexBuffer);
    m_IndexBuffer.InitializeOnGpu(allIndices.data(), allIndices.size() * sizeof(uint32_t));
}
