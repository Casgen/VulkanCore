#pragma once

#include "glm/ext/vector_float3.hpp"
#include "AABB.h"
#include <cstdint>

struct IndexedTriangle
{
    Vec3f a, b, c;
    const uint32_t indexA, indexB, indexC = 0;

    IndexedTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const uint32_t indexA,
             const uint32_t indexB, const uint32_t indexC)
        : a(Vec3f(a.x, a.y, a.z)), b(Vec3f(b.x, b.y, b.z)), c(Vec3f(c.x, c.y, c.z)), indexA(indexA), indexB(indexB), indexC(indexC)
    {
    }

    AABB ComputeAABB() const;
    bool Intersects(const AABB& aabb) const;
};
