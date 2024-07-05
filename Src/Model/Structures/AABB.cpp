#include "Model/Structures/AABB.h"
#include <algorithm>

bool AABB::IsPointInside(const Vec3f& point) const
{

    __m128 aabbMax = _mm_set_ps(0.f, maxPoint.z, maxPoint.y, maxPoint.x);
    __m128 aabbMin = _mm_set_ps(0.f, minPoint.z, minPoint.y, minPoint.x);
    __m128 testedPoint = _mm_set_ps(0.f, point.z, point.y, point.x);

    __m128 maxResult = _mm_cmp_ps(testedPoint, aabbMax, _CMP_LE_OQ);
    __m128 minResult = _mm_cmp_ps(testedPoint, aabbMin, _CMP_GE_OQ);

    int minMask = _mm_movemask_ps(minResult) & 0b0111;
    int maxMask = _mm_movemask_ps(maxResult) & 0b0111;

    // Should we keep this in order to support non-SIMD compatible CPUs?
    // return minPoint.x < point.x && point.x < maxPoint.x && minPoint.y < point.y && point.y < maxPoint.y &&
    //        minPoint.z < point.z && point.z < maxPoint.z;
    //

    return (minMask == 0b0111) && (maxMask == 0b0111);
}

Vec3f AABB::CenterPoint() const
{
    return (maxPoint + minPoint) / Vec3f(2.0);
}

Vec3f AABB::Dimensions() const
{
    return maxPoint - minPoint;
}

bool AABB::Intersects(const AABB& aabb) const
{
    const bool xAxis = minPoint.x <= aabb.minPoint.x && aabb.minPoint.x <= maxPoint.x ||
                       minPoint.x <= aabb.maxPoint.x && aabb.maxPoint.x <= maxPoint.x; // Test in x-axis.
    const bool yAxis = (minPoint.y <= aabb.minPoint.y && aabb.minPoint.y <= maxPoint.y ||
                        minPoint.y <= aabb.maxPoint.y && aabb.maxPoint.y <= maxPoint.y); // Test in y-axis.
    const bool zAxis = (minPoint.z <= aabb.minPoint.z && aabb.minPoint.z <= maxPoint.z ||
                        minPoint.z <= aabb.maxPoint.z && aabb.maxPoint.z <= maxPoint.z); // Test in z-axis.
	
	return xAxis && yAxis && zAxis;
}

std::vector<Edge> AABB::GenerateEdges() const
{

    std::vector<Edge> edges;

    const glm::vec3 a{minPoint.x, minPoint.y, minPoint.z};
    const glm::vec3 b{maxPoint.x, minPoint.y, minPoint.z};
    const glm::vec3 c{maxPoint.x, minPoint.y, maxPoint.z};
    const glm::vec3 d{minPoint.x, minPoint.y, maxPoint.z};

    const glm::vec3 e{minPoint.x, maxPoint.y, minPoint.z};
    const glm::vec3 f{maxPoint.x, maxPoint.y, minPoint.z};
    const glm::vec3 g{maxPoint.x, maxPoint.y, maxPoint.z};
    const glm::vec3 h{minPoint.x, maxPoint.y, maxPoint.z};

    // Bottom Face
    edges.emplace_back(a, b);
    edges.emplace_back(b, c);
    edges.emplace_back(c, d);
    edges.emplace_back(d, a);

    // Top Face
    edges.emplace_back(e, f);
    edges.emplace_back(f, g);
    edges.emplace_back(g, h);
    edges.emplace_back(h, e);

    // Edges connecting bottom and top
    edges.emplace_back(a, e);
    edges.emplace_back(b, f);
    edges.emplace_back(c, g);
    edges.emplace_back(d, h);

    return edges;
}
