
#include <cstddef>
#include <cstdint>
#include <queue>
#include <unordered_map>
#include <vector>
#include "VertexTriangleAdjacency.h"

class MeshUtils
{

  public:
    static VertexTriangleAdjacency BuildVertexTriangleAdjacency(const std::vector<uint32_t>& indices,
                                                                const size_t numOfVertices);

    static std::vector<uint32_t> Tipsify(const std::vector<uint32_t>& indices, const uint32_t vertexCount,
                                         const uint32_t cacheSize);

    static int GetNextVertex(const uint32_t vertexCount, uint32_t i, const uint32_t cacheSize,
                             const std::unordered_map<uint32_t, bool>& candidates,
                             const std::vector<uint32_t>& timeStamps, uint32_t& timeStamp,
                             const std::vector<uint32_t>& liveTriangles, std::queue<uint32_t>& stack);

    static int SkipDeadEnd(const std::vector<uint32_t>& liveTriangles, std::queue<uint32_t>& stack,
                           const uint32_t vertexCount, uint32_t i);

    static uint32_t PackTriangleIntoUInt(const uint32_t a, const uint32_t b, const uint32_t c);
    static uint32_t UnpackTriangleFromUInt(const uint32_t triangle);
};
