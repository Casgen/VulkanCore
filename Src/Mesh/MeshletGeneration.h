#pragma once

#include <cstdint>
#include <vector>

#include "Mesh/MeshVertex.h"
#include "Meshlet.h"
#include "src/meshoptimizer.h"

class MeshletGeneration
{

  public:
    static std::vector<NewMeshlet> MeshletizeNv(uint32_t maxVerts, uint32_t maxIndices,
                                                const std::vector<uint32_t>& indices, const uint32_t verticesSize,
                                                std::vector<uint32_t>& outVertices, std::vector<uint32_t>& outIndices);

    static std::vector<Meshlet> MeshletizeUnoptimized(uint32_t maxVerts, uint32_t maxIndices,
                                                      const std::vector<uint32_t>& indices,
                                                      const uint32_t verticesCount);

    /**
     * It is assumed that the indices are packed as trinagles. Not individual vertex indices!
     */
    static std::vector<MeshletBounds> ComputeMeshletBounds(const std::vector<MeshVertex>& meshVertices, const std::vector<uint32_t>& meshletVertices, const std::vector<NewMeshlet>& meshlets);
};
