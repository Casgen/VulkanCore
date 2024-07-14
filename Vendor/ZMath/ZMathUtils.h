#pragma once

#include "Vec3f.h"
#include "glm/ext/vector_float3.hpp"

class ZMathUtils
{

  public:
    /**
     * Rotates a point around an axis by the defined angle.
	 * @param vec - rotated point.
	 * @param axis - axis which the point is getting rotate around.
	 * @param angle - Angle is in radians.
     */
    static Vec3f RotateVecAroundAxis(const Vec3f& vec, const Vec3f& axis, const float angle);

    /**
     * Rotates a point around an axis by the defined angle.
	 * @param vec - rotated point.
	 * @param axis - axis which the point is getting rotate around.
	 * @param angle - Angle is in radians.
     */
    static glm::vec3 RotateVecAroundAxis(const glm::vec3& vec, const glm::vec3& axis, const float angle);
};
