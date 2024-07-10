
#include <cstddef>
#include <cstdint>
#include <vector>
#include "VertexTriangleAdjacency.h"

class MeshUtils
{

  public:
    static VertexTriangleAdjacency BuildVertexTriangleAdjecency(const std::vector<uint32_t>& indices, const size_t numOfVertices);
};
