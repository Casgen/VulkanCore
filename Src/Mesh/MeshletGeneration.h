#pragma once

#include <cstdint>
#include <vector>

#include "Mesh/Mesh.h"
#include "Meshlet.h"

class MeshletGeneration
{

  public:
    static std::vector<NewMeshlet> MeshletizeNv(uint32_t maxVerts, uint32_t maxIndices,
                                                const std::vector<uint32_t>& indices, const uint32_t verticesSize,
                                                std::vector<uint32_t>& outVertices, std::vector<uint32_t> outIndices);

    static std::vector<Meshlet> MeshletizeUnoptimized(uint32_t maxVerts, uint32_t maxIndices,
                                                      const std::vector<uint32_t>& indices,
                                                      const uint32_t verticesCount);

    static std::vector<Meshlet> OcTreeMeshletizeMesh(uint32_t maxVerts, uint32_t maxIndices, const Mesh& mesh);

    static std::vector<NewMeshlet> TipsifyAndMeshlet(uint32_t maxVerts, uint32_t maxIndices,
                                                     const std::vector<uint32_t>& indices,
                                                     const uint32_t verticesCount);

    static std::vector<uint32_t> Tipsify(const std::vector<uint32_t>& indices, const uint32_t vertexCount,
                                         const uint32_t cacheSize);

    static int GetNextVertex(const uint32_t vertexCount, uint32_t& i, const uint32_t cacheSize,
                             const std::vector<uint32_t>& candidates, const std::vector<uint32_t>& timeStamps,
                             uint32_t& timeStamp, const std::vector<uint32_t>& liveTriangles,
                             std::vector<uint32_t>& stack);

    static int SkipDeadEnd(const std::vector<uint32_t>& liveTriangles, std::vector<uint32_t>& stack,
                           const uint32_t vertexCount, uint32_t& i);
};
