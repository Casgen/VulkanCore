#include "MeshUtils.h"
#include <cstring>
#include <queue>
#include <unordered_map>

VertexTriangleAdjacency MeshUtils::BuildVertexTriangleAdjacency(const std::vector<uint32_t>& indices,
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
    adjList.resize(offset);

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
        .vertexCount = vertexCount,
        .offsets = offsets,
        .adjacencyList = adjList,
    };

	
	// uint32_t* vertexCounts = (uint32_t*) std::calloc(numOfVertices, sizeof(uint32_t));
	//
	// ASSERT(vertexCounts != nullptr, "Failed to allocate vertexCounts!")
	//
	//
	// for (size_t i = 0; i < indices.size(); i++){
	// 	vertexCounts[indices[i]]++;
	// }
	//
	//
	// uint32_t* offsets = (uint32_t*)(std::calloc(numOfVertices, sizeof(uint32_t)));
	// ASSERT(offsets != nullptr, "Tipsify: Failed to allocate offsets!")
	//
	// uint32_t accOffset = 0;
	//
	// for (size_t i = 0; i < numOfVertices; i++) {
	// 	offsets[i] = accOffset;
	// 	accOffset += vertexCounts[i];
	// }
	//
	// uint32_t* copyOffsets = (uint32_t*)(std::calloc(numOfVertices, sizeof(uint32_t)));
	// ASSERT(copyOffsets != nullptr, "Tipsify: Failed to allocate copyOffsets!")
	//
	// std::memcpy(copyOffsets, offsets, sizeof(uint32_t) * numOfVertices);
	//
	// uint32_t adjLength = offsets[numOfVertices - 1] + 1;
	// uint32_t* adjList = (uint32_t*)(std::calloc(adjLength, sizeof(uint32_t)));
	//
	// ASSERT(adjList != nullptr, "Tipsify: Failed to allocate adjacency list!")
	//
	// for (size_t i = 0; i < indices.size(); i += 3) {
 //        const uint32_t triangleIndex = i / 3;
	//
 //        for (size_t j = 0; j < 3; j++)
 //        {
 //            const uint32_t vertexIndex = indices[i + j];
 //            const uint32_t position = copyOffsets[vertexIndex];
	//
 //            adjList[position] = triangleIndex;
 //            copyOffsets[vertexIndex]++;
 //        }
	// }
	//
	// std::vector<uint32_t> vecOffsets(numOfVertices);
	// vecOffsets.resize(numOfVertices);
	//
	// std::vector<uint32_t> vecAdjList;
	// vecAdjList.resize(adjLength);
	//
	// std::vector<uint32_t> vecVertexCounts;
	// vecVertexCounts.resize(numOfVertices);
	//
	// std::memcpy(vecOffsets.data(), offsets, sizeof(uint32_t) * numOfVertices);
	// std::memcpy(vecAdjList.data(), adjList, sizeof(uint32_t) * adjLength);
	// std::memcpy(vecVertexCounts.data(), vertexCounts, sizeof(uint32_t) * numOfVertices);
	//
	// std::free(offsets);
	// std::free(vertexCounts);
	// std::free(copyOffsets);
	// std::free(adjList);
	//
	// return {
	// 	.indices = indices,
	// 	.vertexCount = vecVertexCounts,
	// 	.offsets = vecOffsets,
	// 	.adjacencyList = vecAdjList
	// };
}

std::vector<uint32_t> MeshUtils::Tipsify(const std::vector<uint32_t>& indices, const uint32_t vertexCount,
                                         const uint32_t cacheSize)
{
    VertexTriangleAdjacency adj = MeshUtils::BuildVertexTriangleAdjacency(indices, vertexCount);

    std::vector<uint32_t> liveTriangles(adj.vertexCount);
    std::vector<uint32_t> cachingTimeStamps(vertexCount);
    std::queue<uint32_t> deadEndStack;

    std::vector<bool> emmitedTriangles(indices.size() / 3);

    int f = 0;

    uint32_t timeStamp = cacheSize + 1;
    uint32_t cursor = 1;

    std::vector<uint32_t> outputIndices;

    while (f >= 0)
    {
        std::unordered_map<uint32_t, bool> ringCandidates;

        std::vector<VertexTriangleAdjacency::Triangle> triangles =
            adj.GetTriangles(f); // Obtains the triangles with the given indices.
        std::vector<uint32_t> triangleIndices =
            adj.GetTriangleIndices(f); // Indexes into the index buffer. Points at the different triangles.

        for (size_t i = 0; i < triangles.size(); i++)
        {
            if (!emmitedTriangles[triangleIndices[i]])
            {
                for (size_t t = 0; t < 3; t++)
                {
                    const uint32_t v = triangles[i].vertices[t];
                    outputIndices.emplace_back(v);
                    deadEndStack.push(v);
                    ringCandidates.emplace(v, true);

                    liveTriangles[v]--;

                    if (timeStamp - cachingTimeStamps[v] > cacheSize)
                    {
                        cachingTimeStamps[v] = timeStamp++;
                    }
                }
                emmitedTriangles[triangleIndices[i]] = true;
            }
        }

        // Get next fanning vertex ----
        f = MeshUtils::GetNextVertex(vertexCount, cursor, cacheSize, ringCandidates, cachingTimeStamps, timeStamp,
                                     liveTriangles, deadEndStack);
    }

    return outputIndices;
}

int MeshUtils::GetNextVertex(const uint32_t vertexCount, uint32_t i, const uint32_t cacheSize,
                             const std::unordered_map<uint32_t, bool>& candidates,
                             const std::vector<uint32_t>& timeStamps, uint32_t& timeStamp,
                             const std::vector<uint32_t>& liveTriangles, std::queue<uint32_t>& stack)
{
    int bestCandidate = -1;
    int priority = -1;
    int prevPriority = 0;

    for (const auto& pair : candidates)
    {
        const uint32_t candidate = pair.first;
        if (liveTriangles[candidate] > 0)
        {

            priority = 0;

            if (timeStamp - timeStamps[candidate] + 2 * liveTriangles[candidate] <= cacheSize)
            {
                priority = timeStamp - timeStamps[candidate];
            }

            if (priority > prevPriority)
            {
                prevPriority = priority;
                bestCandidate = candidate;
            }
        }
    }

    if (bestCandidate == -1)
    {
        bestCandidate = MeshUtils::SkipDeadEnd(liveTriangles, stack, vertexCount, i);
    }

    return bestCandidate;
}

int MeshUtils::SkipDeadEnd(const std::vector<uint32_t>& liveTriangles, std::queue<uint32_t>& stack,
                           const uint32_t vertexCount, uint32_t i)
{
    while (!stack.empty())
    {

        const uint32_t d = stack.back();
        stack.pop();

        if (liveTriangles[d] > 0)
        {
            return d;
        }
    }

    while (i < liveTriangles.size())
    {
        if (liveTriangles[i] > 0)
        {
            return i;
        }

        i++;
    }

    return -1;
}

uint32_t MeshUtils::PackTriangleIntoUInt(const uint32_t a, const uint32_t b, const uint32_t c)
{
    return (a & 0xFF) | ((b & 0xFF) << 8) | ((c & 0xFF) << 16);
}

uint32_t MeshUtils::UnpackTriangleFromUInt(const uint32_t triangle)
{
    return (triangle & 0xFF) | ((triangle >> 8) & 0xFF) | ((triangle >> 16) & 0xFF);
}
