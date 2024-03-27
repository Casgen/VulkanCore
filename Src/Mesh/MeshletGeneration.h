#pragma once

#include <cstdint>
#include <vector>

#include "Meshlet.h"
#include "glm/vec3.hpp"

class MeshletGeneration {

   public:
    static std::vector<Meshlet> MeshletizeUnoptimized(uint32_t maxVerts, uint32_t maxIndices,
                                                      const std::vector<uint32_t> indices,
                                                      const uint32_t verticesCount);
};
