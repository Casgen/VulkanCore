#include "ZMathUtils.h"
#include "glm/geometric.hpp"

Vec3f ZMathUtils::RotateVecAroundAxis(const Vec3f& vec, const Vec3f& axis, const float angle)
{
    Vec3f normalAxis = axis.Normalize();
    return vec * cosf(angle) + normalAxis * vec.Dot(normalAxis) * (1 - cosf(angle)) +
           vec.Cross(normalAxis) * sinf(angle);
}

glm::vec3 ZMathUtils::RotateVecAroundAxis(const glm::vec3& vec, const glm::vec3& axis, const float angle)
{
    glm::vec3 normalAxis = glm::normalize(axis);
    return vec * cosf(angle) + normalAxis * glm::dot(vec, normalAxis) * (1 - cosf(angle)) +
           glm::cross(vec, normalAxis) * sinf(angle);
}
