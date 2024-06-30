#pragma once

#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_uint3.hpp"
#include "AABB.h"
#include <cstdint>

struct IndexedTriangle
{
    glm::vec3 a, b, c;
    glm::uvec3 indices = {0, 1, 2};

    IndexedTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const uint32_t indexA,
             const uint32_t indexB, const uint32_t indexC)
        : a(a), b(b), c(c), indices({indexA, indexB, indexC})
    {
    }

    AABB ComputeAABB() const;
    bool Overlaps(const AABB& aabb) const;
};
