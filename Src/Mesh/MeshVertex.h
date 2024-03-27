#pragma once

#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"

struct MeshVertex {
    glm::vec3 Position;
    alignas(16) glm::vec3 Normal;
    alignas(16) glm::vec3 Tangent;
    alignas(16) glm::vec3 BiTangent;
    glm::vec2 TexCoords;
};
