#include "Triangle.h"
#include <cmath>
#include <immintrin.h>

AABB Triangle::ComputeAABB() const
{

    __m128 amm = _mm_set_ps(0.f, a.z, a.y, a.x);
    __m128 bmm = _mm_set_ps(0.f, b.z, b.y, b.x);
    __m128 cmm = _mm_set_ps(0.f, c.z, c.y, c.x);

    __m128 maxPoint = _mm_max_ps(amm, bmm);
    maxPoint = _mm_max_ps(maxPoint, cmm);

    __m128 minPoint = _mm_min_ps(amm, bmm);
    minPoint = _mm_min_ps(minPoint, cmm);

    // Should we keep this in order to support non-SIMD compatible CPUs?
    // maxPoint.x = std::max(maxPoint.x, b.x);
    // maxPoint.y = std::max(maxPoint.y, b.y);
    // maxPoint.z = std::max(maxPoint.z, b.z);
    // maxPoint.x = std::max(maxPoint.x, c.x);
    // maxPoint.y = std::max(maxPoint.y, c.y);
    // maxPoint.z = std::max(maxPoint.z, c.z);

    // minPoint.x = std::min(minPoint.x, b.x);
    // minPoint.y = std::min(minPoint.y, b.y);
    // minPoint.z = std::min(minPoint.z, b.z);
    // minPoint.x = std::min(minPoint.x, c.x);
    // minPoint.y = std::min(minPoint.y, c.y);
    // minPoint.z = std::min(minPoint.z, c.z);

    // We must be careful when obtaining the values.
    // The values in MMX registers are stored in reversed order!
    return AABB{
        .minPoint = glm::vec3(minPoint[0], minPoint[1], minPoint[2]),
        .maxPoint = glm::vec3(maxPoint[0], maxPoint[1], maxPoint[2]),
    };
}

bool Triangle::Overlaps(const AABB& aabb) const {

	return aabb.IsPointInside(a) || aabb.IsPointInside(b) || aabb.IsPointInside(c);
}
