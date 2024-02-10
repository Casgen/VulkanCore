#pragma once

#include "glm/vec3.hpp"
#include <cstdint>
#include <vector>
#include "Meshlet.h"

class MeshletGeneration
{

  public:

    // This Function takes all the vertices and indices and creates the individual meshlets
    // There is not optimization involved in this. It basically loops through the arrays and puts them in the meshlets
    static std::vector<Meshlet> MeshletizeUnoptimized(uint32_t maxVerts, uint32_t maxIndices, const std::vector<uint32_t> indices);
};
