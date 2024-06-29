#pragma once

#include "glm/ext/vector_float3.hpp"
#include "AABB.h"

struct Triangle
{
    glm::vec3 a, b, c;
	size_t id;

    Triangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3 c, const size_t id = 0) : a(a), b(b), c(c), id(id)
    {
    }
    AABB ComputeAABB() const;
	bool Overlaps(const AABB& aabb) const;
};
