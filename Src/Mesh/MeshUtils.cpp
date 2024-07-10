#include "MeshUtils.h"
#include <cstring>

VertexTriangleAdjacency MeshUtils::BuildVertexTriangleAdjecency(const std::vector<uint32_t>& indices,
                                                                size_t numOfVertices)
{
    std::vector<uint32_t> vertexCount;

    vertexCount.resize(numOfVertices);

    for (size_t i = 0; i < indices.size(); i++)
    {
        vertexCount[indices[i]]++;
    }

    std::vector<uint32_t> offsets;
	offsets.resize(numOfVertices);

    uint32_t offset = 0;

    for (size_t i = 0; i < vertexCount.size(); i++)
    {
        offsets[i] = offset;
        offset += vertexCount[i];
    }

    std::vector<uint32_t> copyOffsets(offsets);

    std::vector<uint32_t> adjList;
    adjList.resize(offsets[offsets.size() - 1] + 1);

    for (std::size_t i = 0; i < indices.size(); i += 3)
    {

		const uint32_t triangleIndex = i / 3;
        for (size_t j = 0; j < 3; j++)
        {
            const uint32_t vertexIndex = indices[i + j];
            const uint32_t position = copyOffsets[vertexIndex];

            adjList[position] = triangleIndex;
            copyOffsets[vertexIndex]++;
        }
    }

    return {
		.indices = indices,
        .vertexCount = std::move(vertexCount),
        .offsets = std::move(offsets),
        .adjacencyList = std::move(adjList),
    };
}
