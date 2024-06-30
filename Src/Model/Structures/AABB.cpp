#include "Model/Structures/AABB.h"
#include <algorithm>

bool AABB::IsPointInside(const glm::vec3 point) const
{

    __m256 aabbPoints = _mm256_set_ps(minPoint.x, minPoint.y, minPoint.z, maxPoint.x, maxPoint.y, maxPoint.z, 0.0, 0.0);
    __m256 testedPoint = _mm256_set_ps(point.x, point.y, point.z, point.x, point.y, point.z, 0.0, 0.0);

    __m256 minResult = _mm256_cmp_ps(aabbPoints, testedPoint, _CMP_GE_OQ);
    __m256 maxResult = _mm256_cmp_ps(aabbPoints, testedPoint, _CMP_LE_OQ);

    int minMask = _mm256_movemask_ps(minResult);
    int maxMask = _mm256_movemask_ps(maxResult);

    // Should we keep this in order to support non-SIMD compatible CPUs?
    // return minPoint.x < point.x && point.x < maxPoint.x && minPoint.y < point.y && point.y < maxPoint.y &&
    //        minPoint.z < point.z && point.z < maxPoint.z;
    //

    return (minMask == 31) && (maxMask == 227);
}

glm::vec3 AABB::CenterPoint() const
{
    return (maxPoint + minPoint) / glm::vec3(2.0);
}

glm::vec3 AABB::Dimensions() const
{
    return maxPoint - minPoint;
}

bool AABB::IsOverlapping(const AABB& aabb) const
{
    return IsPointInside(aabb.minPoint) || IsPointInside(aabb.maxPoint);
}

std::vector<Edge> AABB::GenerateEdges() const  {

	std::vector<Edge> edges;
	
	const glm::vec3 b{maxPoint.x, minPoint.y, minPoint.z};
	const glm::vec3 c{maxPoint.x, minPoint.y, maxPoint.z};
	const glm::vec3 d{minPoint.x, minPoint.y, maxPoint.z};

	const glm::vec3 e{minPoint.x, maxPoint.y, minPoint.z};
	const glm::vec3 f{maxPoint.x, maxPoint.y, minPoint.z};
	const glm::vec3 h{minPoint.x, maxPoint.y, maxPoint.z};

	// Bottom Face
	edges.emplace_back(minPoint, b);
	edges.emplace_back(b, c);
	edges.emplace_back(c, d);
	edges.emplace_back(d, minPoint);

	// Top Face
	edges.emplace_back(e, f);
	edges.emplace_back(f, maxPoint);
	edges.emplace_back(maxPoint, h);
	edges.emplace_back(h, e);

	// Edges connecting bottom and top
	edges.emplace_back(minPoint, e);
	edges.emplace_back(b, f);
	edges.emplace_back(c, maxPoint);
	edges.emplace_back(d, h);

	return edges;
}
