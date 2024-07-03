#include "IndexedTriangle.h"
#include <cmath>
#include <cstdlib>
#include <immintrin.h>

AABB IndexedTriangle::ComputeAABB() const
{

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

    return AABB{.minPoint = Vec3f::Min(Vec3f::Min(a, b), c), .maxPoint = Vec3f::Max(Vec3f::Max(a, b), c)};
}

bool IndexedTriangle::Intersects(const AABB& aabb) const
{

    if (!aabb.Intersects(ComputeAABB()))
    {
        return false;
    }

    Vec3f extent = aabb.Dimensions() / 2;
    Vec3f boxCenter = aabb.CenterPoint();

    Vec3f aCenter = a - boxCenter;
    Vec3f bCenter = b - boxCenter;
    Vec3f cCenter = c - boxCenter;

    Vec3f f0 = bCenter - aCenter;
    Vec3f f1 = cCenter - bCenter;
    Vec3f f2 = aCenter - cCenter;

    Vec3f u0 = Vec3f(1.f, 0.f, 0.f);
    Vec3f u1 = Vec3f(0.f, 1.f, 0.f);
    Vec3f u2 = Vec3f(0.f, 0.f, 1.f);

    Vec3f firstAxis[9] = {};

    firstAxis[0] = u0.Cross(f0);
    firstAxis[1] = u0.Cross(f1);
    firstAxis[2] = u0.Cross(f2);

    firstAxis[3] = u1.Cross(f0);
    firstAxis[4] = u1.Cross(f1);
    firstAxis[5] = u1.Cross(f2);

    firstAxis[6] = u2.Cross(f0);
    firstAxis[7] = u2.Cross(f1);
    firstAxis[8] = u2.Cross(f2);

    for (int i = 0; i < 9; i++)
    {
        float p0 = aCenter.Dot(firstAxis[i]);
        float p1 = bCenter.Dot(firstAxis[i]);
        float p2 = cCenter.Dot(firstAxis[i]);

        float r = extent.x * abs(u0.Dot(firstAxis[i])) + extent.y * abs(u1.Dot(firstAxis[i])) +
                  extent.x * abs(u2.Dot(firstAxis[i]));

        float pMax1 = std::max(p0, p1);
        float pMin1 = std::min(p0, p1);

        float pMax2 = std::max(pMax1, p1);
        float pMin2 = std::min(pMin1, p1);

        if (std::max(-pMax2, pMin2) > r)
        {
            return false;
        }
    }

    Vec3f triangleNormal = f0.Cross(f2).Normalize();

    float p0 = aCenter.Dot(triangleNormal);
    float p1 = bCenter.Dot(triangleNormal);
    float p2 = cCenter.Dot(triangleNormal);

    float r = extent.x * abs(u0.Dot(triangleNormal)) + extent.y * abs(u1.Dot(triangleNormal)) +
              extent.x * abs(u2.Dot(triangleNormal));

    float pMax1 = std::max(p0, p1);
    float pMin1 = std::min(p0, p1);

    float pMax2 = std::max(pMax1, p1);
    float pMin2 = std::min(pMin1, p1);

    if (std::max(-pMax2, pMin2) > r)
    {
        return false;
    }

    return true;
}
