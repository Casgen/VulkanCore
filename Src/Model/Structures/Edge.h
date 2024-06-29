#pragma once

#include "glm/ext/vector_float3.hpp"

struct Edge {
	glm::vec3 a, b;

	Edge(const glm::vec3& a, const glm::vec3& b) : a(a), b(b) {

	}
};
