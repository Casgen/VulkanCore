#pragma once

#include <cstdint>
#include <vector>

#include "Mesh/Mesh.h"
#include "Meshlet.h"

class MeshletGeneration
{

  public:
    static std::vector<Meshlet> MeshletizeUnoptimized(uint32_t maxVerts, uint32_t maxIndices,
                                                      const std::vector<uint32_t>& indices,
                                                      const uint32_t verticesCount);

    static std::vector<Meshlet> OcTreeMeshletizeMesh(uint32_t maxVerts, uint32_t maxIndices, const Mesh& mesh);
};
