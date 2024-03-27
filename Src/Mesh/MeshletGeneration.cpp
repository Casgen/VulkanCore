
#include "MeshletGeneration.h"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <unordered_map>

#include "glm/common.hpp"

// std::vector<Meshlet> MeshletGeneration::MeshletizeUnoptimized(uint32_t maxVerts, uint32_t maxIndices,
//                                                               const std::vector<uint32_t> indices)
// {
//
//     Meshlet meshlet;
//
//     std::vector<Meshlet> meshlets;
//
//     std::vector<uint32_t> index_stack, vertex_stack;
//
//     index_stack.reserve(maxIndices);
//
//     for (uint32_t offset = 0; offset < indices.size();)
//     {
//
//         meshlet = {};
//
//         size_t offsetEnd = glm::min(size_t(offset + maxIndices), indices.size());
//
//         uint32_t triangleCount = 0;
//
//         for (uint32_t i = offset; i < offsetEnd; i++)
//         {
//             triangleCount += uint32_t((i + 1) % 3 == 0);
//
//             bool isVertexPresent = false;
//
//             for (uint32_t j = 0; j < vertex_stack.size(); j++)
//             {
//                 isVertexPresent = indices[i] == vertex_stack[j];
//                 if (isVertexPresent)
//                     break;
//             }
//
//             if (!isVertexPresent)
//             {
//                 if (maxVerts <= vertex_stack.size())
//                     break;
//
//                 vertex_stack.emplace_back(indices[i]);
//             }
//
//             index_stack.push_back(indices[i]);
//         }
//
//         assert(maxVerts >= vertex_stack.size());
//
//         uint32_t i = 0;
//
//         for (auto& vert : vertex_stack)
//         {
//             meshlet.vertices[i] = vert;
//             i++;
//         }
//
//         assert(maxIndices >= index_stack.size());
//
//         meshlet.vertexCount = vertex_stack.size();
//
//         for (uint32_t i = 0; i < triangleCount * 3; i++)
//         {
//             meshlet.indices[i] = index_stack[i];
//         }
//
//         meshlet.indicesCount = index_stack.size();
//
//         meshlets.push_back(meshlet);
//
//         index_stack.clear();
//         vertex_stack.clear();
//
//         offset += triangleCount * 3;
//     }
//
//     return meshlets;
// }

std::vector<Meshlet> MeshletGeneration::MeshletizeUnoptimized(uint32_t maxVerts, uint32_t maxIndices,
                                                              const std::vector<uint32_t> indices,
                                                              const uint32_t verticesSize) {
    std::vector<Meshlet> meshlets;
    Meshlet meshlet = {};

    std::vector<uint8_t> vertices(verticesSize, 0xFF);

    uint8_t vertexCount = 0;

    for (uint32_t i = 0; i < indices.size(); i += 3) {

        uint32_t a = indices[i + 0];
        uint32_t b = indices[i + 1];
        uint32_t c = indices[i + 2];

        uint8_t& av = vertices[a];
        uint8_t& bv = vertices[b];
        uint8_t& cv = vertices[c];

        if ((meshlet.vertexCount + (av == 0xFF) + (bv == 0xFF) + (cv == 0xFF) > maxVerts) ||
            (meshlet.indicesCount + 3 > maxIndices)) {
            meshlets.push_back(meshlet);
            meshlet = {};
            memset(vertices.data(), 0xFF, verticesSize);
        }

        if (av == 0xFF) {
            av = meshlet.vertexCount;
            meshlet.vertices[meshlet.vertexCount++] = a;
        }

        if (bv == 0xFF) {
            bv = meshlet.vertexCount;
            meshlet.vertices[meshlet.vertexCount++] = b;
        }

        if (cv == 0xFF) {
            cv = meshlet.vertexCount;
            meshlet.vertices[meshlet.vertexCount++] = c;
        }

        meshlet.indices[meshlet.indicesCount++] = av;
        meshlet.indices[meshlet.indicesCount++] = bv;
        meshlet.indices[meshlet.indicesCount++] = cv;
    }

    if (meshlet.indicesCount != 0) {
        meshlets.push_back(meshlet);
    }

    return meshlets;
}
