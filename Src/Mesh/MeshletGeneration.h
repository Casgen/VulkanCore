#pragma once

#include <cstdint>
#include <vector>

#include "Meshlet.h"

class MeshletGeneration
{

  public:
    static std::vector<NewMeshlet> MeshletizeNv(uint32_t maxVerts, uint32_t maxIndices,
                                                const std::vector<uint32_t>& indices, const uint32_t verticesSize,
                                                std::vector<uint32_t>& outVertices, std::vector<uint32_t>& outIndices);

    static std::vector<Meshlet> MeshletizeUnoptimized(uint32_t maxVerts, uint32_t maxIndices,
                                                      const std::vector<uint32_t>& indices,
                                                      const uint32_t verticesCount);
};
