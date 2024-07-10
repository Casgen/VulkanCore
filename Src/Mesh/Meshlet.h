#pragma once

#include <cstdint>
#include "../Constants.h"
#include "glm/ext/vector_float3.hpp"

struct Meshlet
{
    uint32_t indices[Constants::MAX_MESHLET_INDICES] = {};
    uint32_t vertices[Constants::MAX_MESHLET_VERTICES] = {};
    uint32_t vertexCount = 0;
    uint32_t indicesCount = 0;
};

struct NewMeshlet
{
	uint32_t vertexOffset = 0;
	uint32_t triangleOffset = 0;
	uint32_t vertexCount = 0;
	uint32_t triangleCount = 0;
};

struct MeshletBounds
{
	glm::vec3 normal;
	float coneAngle;
	glm::vec3 spherePos;
	float sphereRadius;

	// We have to be carefull around the std430 layout since it is 4-base. (16 bytes)
	// and in the array it would throw off the offsets
	alignas(16) glm::vec3 normalPos;
};
