#pragma once

#include "Vk/Buffers/Buffer.h"
#include "vulkan/vulkan_enums.hpp"
#include <cstdint>
#include <vector>

template <typename V>
struct SceneModel {
	
	std::vector<uint32_t> indices;
	std::vector<V> vertices;

	VkCore::Buffer indexBuffer;
	VkCore::Buffer vertexBuffer;

	void CreateBuffers() {
		indexBuffer = VkCore::Buffer(vk::BufferUsageFlagBits::eIndexBuffer);
		indexBuffer.InitializeOnGpu(indices.data(), indices.size() * sizeof(uint32_t));

		vertexBuffer = VkCore::Buffer(vk::BufferUsageFlagBits::eVertexBuffer);
		vertexBuffer.InitializeOnGpu(vertices.data(), vertices.size() * sizeof(V));
	}
};
