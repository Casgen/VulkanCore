#pragma once

#define GLM_FORCE_SSE2

#include "Model/Structures/Edge.h"
#include "glm/ext/vector_float3.hpp"
#include <immintrin.h>
#include <vector>
#include <xmmintrin.h>

struct AABB
{

    glm::vec3 minPoint = glm::vec3(-1);
    glm::vec3 maxPoint = glm::vec3(1);

    AABB() = default;

    bool IsPointInside(const glm::vec3 point) const;
    glm::vec3 CenterPoint() const;
    glm::vec3 Dimensions() const;
    bool IsOverlapping(const AABB& aabb) const;
    std::vector<Edge> GenerateEdges() const;
};
