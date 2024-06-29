
#include "MeshletGeneration.h"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <immintrin.h>
#include <limits>
#include <xmmintrin.h>

#include "Constants.h"
#include "Log/Log.h"
#include "Model/Structures/OcTree.h"
#include "Model/Structures/Triangle.h"
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
                                                              const std::vector<uint32_t>& indices,
                                                              const uint32_t verticesSize)
{
    std::vector<Meshlet> meshlets;
    Meshlet meshlet = {};

    std::vector<uint8_t> vertices(verticesSize, 0xFF);

    uint8_t vertexCount = 0;

    for (uint32_t i = 0; i < indices.size(); i += 3)
    {

        uint32_t a = indices[i + 0];
        uint32_t b = indices[i + 1];
        uint32_t c = indices[i + 2];

        uint8_t& av = vertices[a];
        uint8_t& bv = vertices[b];
        uint8_t& cv = vertices[c];

        if ((meshlet.vertexCount + (av == 0xFF) + (bv == 0xFF) + (cv == 0xFF) > maxVerts) ||
            (meshlet.indicesCount + 3 > maxIndices))
        {
            meshlets.push_back(meshlet);
            meshlet = {};
            memset(vertices.data(), 0xFF, verticesSize);
        }

        if (av == 0xFF)
        {
            av = meshlet.vertexCount;
            meshlet.vertices[meshlet.vertexCount++] = a;
        }

        if (bv == 0xFF)
        {
            bv = meshlet.vertexCount;
            meshlet.vertices[meshlet.vertexCount++] = b;
        }

        if (cv == 0xFF)
        {
            cv = meshlet.vertexCount;
            meshlet.vertices[meshlet.vertexCount++] = c;
        }

        meshlet.indices[meshlet.indicesCount++] = av;
        meshlet.indices[meshlet.indicesCount++] = bv;
        meshlet.indices[meshlet.indicesCount++] = cv;
    }

    if (meshlet.indicesCount != 0)
    {
        meshlets.push_back(meshlet);
    }

    return meshlets;
}

std::vector<Meshlet> MeshletGeneration::OcTreeMeshletizeMesh(uint32_t maxVerts, uint32_t maxIndices, const Mesh& mesh)
{

    std::vector<Triangle> triangles;
    triangles.reserve(mesh.indices.size() / 3);

    if (mesh.indices.size() % 3 != 0)
    {
        LOGF(Rendering, Warning,
             "The number of indices in the model do not look like triangles! Mesh may not be complete! Indices count: "
             "%d",
             mesh.indices.size())
    }

	for (size_t i = 0; i < mesh.indices.size() - 3; i++) {
		triangles.emplace_back(mesh.vertices[mesh.indices[i]].Position, mesh.vertices[mesh.indices[i + 1]].Position, mesh.vertices[mesh.indices[i + 3]].Position, 1);
	}

    OcTreeTriangles ocTree = OcTreeTriangles(Mesh::CreateBoundingBox(mesh), Constants::MAX_MESHLET_INDICES / 3);

	for (const auto& triangle : triangles) {
		ocTree.Push(triangle);
	}


    std::vector<OcTreeTriangles::Query> queries = ocTree.GetAllNodeTriangles();

    std::vector<Meshlet> meshlets;
    Meshlet meshlet = {};

    for (const auto& query : queries)
    {
        for (const auto& triangle : query.triangles)
        {

            std::vector<uint8_t> vertexLookup(mesh.vertices.size(), 0xFF);

            uint8_t vertexCount = 0;

            uint32_t a = mesh.indices[triangle.id * 3];
            uint32_t b = mesh.indices[triangle.id * 3 + 1];
            uint32_t c = mesh.indices[triangle.id * 3 + 2];

            uint8_t& av = vertexLookup[a];
            uint8_t& bv = vertexLookup[b];
            uint8_t& cv = vertexLookup[c];

            if ((meshlet.vertexCount + (av == 0xFF) + (bv == 0xFF) + (cv == 0xFF) > maxVerts) ||
                (meshlet.indicesCount + 3 > maxIndices))
            {
                meshlets.push_back(meshlet);
                meshlet = {};
                memset(vertexLookup.data(), 0xFF, mesh.vertices.size());
            }

            if (av == 0xFF)
            {
                av = meshlet.vertexCount;
                meshlet.vertices[meshlet.vertexCount++] = a;
            }

            if (bv == 0xFF)
            {
                bv = meshlet.vertexCount;
                meshlet.vertices[meshlet.vertexCount++] = b;
            }

            if (cv == 0xFF)
            {
                cv = meshlet.vertexCount;
                meshlet.vertices[meshlet.vertexCount++] = c;
            }

            meshlet.indices[meshlet.indicesCount++] = av;
            meshlet.indices[meshlet.indicesCount++] = bv;
            meshlet.indices[meshlet.indicesCount++] = cv;
        }

        if (meshlet.indicesCount != 0)
        {
            meshlets.push_back(meshlet);
        }
    }

    return meshlets;
}
