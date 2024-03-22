
#include "MeshletGeneration.h"
#include "glm/common.hpp"
#include <cassert>
#include <cstdint>
#include <unordered_map>

std::vector<Meshlet> MeshletGeneration::MeshletizeUnoptimized(uint32_t maxVerts, uint32_t maxIndices,
                                                              const std::vector<uint32_t> indices)
{

    Meshlet meshlet;

    std::vector<Meshlet> meshlets;

    std::vector<uint32_t> index_stack, vertex_stack;

    index_stack.reserve(maxIndices);

    for (uint32_t offset = 0; offset < indices.size();)
    {

        meshlet = {};

        size_t offsetEnd = glm::min(size_t(offset + maxIndices), indices.size());

        uint32_t triangleCount = 0;

        for (uint32_t i = offset; i < offsetEnd; i++)
        {
            triangleCount += uint32_t((i + 1) % 3 == 0);

            bool isVertexPresent = false;

            for (uint32_t j = 0; j < vertex_stack.size(); j++)
            {
                isVertexPresent = indices[i] == vertex_stack[j];
                if (isVertexPresent)
                    break;
            }

            if (!isVertexPresent)
            {
                if (maxVerts <= vertex_stack.size())
                    break;

                vertex_stack.emplace_back(indices[i]);
            }

            index_stack.push_back(indices[i]);
        }

        assert(maxVerts >= vertex_stack.size());

        uint32_t i = 0;

        for (auto& vert : vertex_stack)
        {
            meshlet.vertices[i] = vert;
            i++;
        }

        assert(maxIndices >= index_stack.size());

        meshlet.vertexCount = vertex_stack.size();

        for (uint32_t i = 0; i < triangleCount * 3; i++)
        {
            meshlet.indices[i] = index_stack[i];
        }

        meshlet.indicesCount = index_stack.size();

        meshlets.push_back(meshlet);

        index_stack.clear();
        vertex_stack.clear();

        offset += triangleCount * 3;
    }

    return meshlets;
}

std::vector<Meshlet> MeshletGeneration::MeshletizeUnoptimizedTest(uint32_t maxVerts, uint32_t maxIndices,
                                                                  const std::vector<uint32_t> indices,
                                                                  const uint32_t verticesSize)
{
    std::vector<Meshlet> meshlets;
    Meshlet meshlet = {};

    std::vector<uint8_t> vertices(verticesSize, 255);

    uint8_t vertexCount = 0;

    for (uint32_t i = 0; i < indices.size(); i += 3)
    {

        uint32_t a = indices[i + 1];
        uint32_t b = indices[i + 2];
        uint32_t c = indices[i + 3];

        // if (vertexCount > maxVerts) {
        //     meshlets.emplace_back(_Args &&__args...)
        // }
    }
}
