#pragma once

#include "Log/Log.h"
#include "Model/Structures/IndexedTriangle.h"
#include <cstdint>
#include <vector>

struct VertexTriangleAdjacency
{

    struct Triangle
    {
        size_t vertices[3] = {0, 0, 0};

        Triangle(const size_t a, const size_t b, const size_t c)
        {
            vertices[0] = a;
            vertices[1] = b;
            vertices[2] = c;
        }
    };

    std::vector<uint32_t> indices;
    std::vector<uint32_t> vertexCount;
    std::vector<uint32_t> offsets;
    std::vector<uint32_t> adjacencyList;

    std::vector<uint32_t> GetTriangleIndices(const size_t vertexIndex)
    {
        const uint32_t offset = offsets[vertexIndex];

        std::vector<uint32_t> triangleIndices;

        for (int i = offset; i < offset + vertexCount[vertexIndex]; i++)
        {
            triangleIndices.push_back(adjacencyList[i]);
        }

        return triangleIndices;
    }

    std::vector<Triangle> GetTriangles(const size_t vertexIndex)
    {
        const uint32_t offset = offsets[vertexIndex];

        std::vector<Triangle> triangles;

        size_t triangleCount = vertexCount[vertexIndex];

        ASSERT(offset < adjacencyList.size() && (offset + triangleCount) <= adjacencyList.size(),
               "Failed to get triangles! Triangle accessing out of bonds of the indices array!");

        for (int i = offset; i < offset + triangleCount; i++)
        {
            const size_t triangleIndex = adjacencyList[i];

            triangles.emplace_back(indices[triangleIndex * 3], indices[triangleIndex * 3 + 1],
                                   indices[triangleIndex * 3 + 2]);
        }

        return triangles;
    }
};
