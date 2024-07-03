#pragma once

#include "Model/Structures/Edge.h"
#include <immintrin.h>
#include <vector>
#include <xmmintrin.h>
#include "../ZMath/Vec3f.h"

struct AABB
{
    Vec3f minPoint = Vec3f(-1.f);
    Vec3f maxPoint = Vec3f(1.f);

    bool IsPointInside(const Vec3f& point) const;
    Vec3f CenterPoint() const;
    Vec3f Dimensions() const;
    bool Intersects(const AABB& aabb) const;
    std::vector<Edge> GenerateEdges() const;
};
