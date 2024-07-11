
#include "MeshletGeneration.h"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <immintrin.h>
#include <xmmintrin.h>

#include "Mesh/Meshlet.h"
#include "MeshUtils.h"

std::vector<NewMeshlet> MeshletGeneration::MeshletizeNv(uint32_t maxVerts, uint32_t maxIndices,
                                                        const std::vector<uint32_t>& indices,
                                                        const uint32_t verticesSize, std::vector<uint32_t>& outVertices,
                                                        std::vector<uint32_t>& outIndices)
{
    outVertices.reserve(verticesSize);
    outIndices.reserve(indices.size());

    std::vector<NewMeshlet> meshlets;
    std::vector<uint8_t> vertices(verticesSize, 0xFF);

    NewMeshlet meshlet;

    for (uint32_t i = 0; i < indices.size(); i += 3)
    {

        uint32_t a = indices[i + 0];
        uint32_t b = indices[i + 1];
        uint32_t c = indices[i + 2];

        uint8_t& av = vertices[a];
        uint8_t& bv = vertices[b];
        uint8_t& cv = vertices[c];

        if ((meshlet.vertexCount + (av == 0xFF) + (bv == 0xFF) + (cv == 0xFF) > maxVerts) ||
            (meshlet.triangleCount + 1 > maxIndices / 3))
        {
            meshlet.triangleOffset = outIndices.size() - meshlet.triangleCount;
            meshlet.vertexOffset = outVertices.size() - meshlet.vertexCount;

            meshlets.push_back(meshlet);
            memset(vertices.data(), 0xFF, verticesSize);

            meshlet = {};
        }

        if (av == 0xFF)
        {
            av = meshlet.vertexCount;
            outVertices.emplace_back(a);
            meshlet.vertexCount++;
        }

        if (bv == 0xFF)
        {
            bv = meshlet.vertexCount;
            outVertices.emplace_back(b);
            meshlet.vertexCount++;
        }

        if (cv == 0xFF)
        {
            cv = meshlet.vertexCount;
            outVertices.emplace_back(c);
            meshlet.vertexCount++;
        }

        uint32_t packedTriangle = (av & 0xFF);
        packedTriangle |= (bv & 0xFF) << 8;
        packedTriangle |= (cv & 0xFF) << 16;

        outIndices.emplace_back(packedTriangle);

        meshlet.triangleCount++;
    }

    if (meshlet.triangleCount != 0)
    {
        meshlet.triangleOffset = outIndices.size() - meshlet.triangleCount;
        meshlet.vertexOffset = outVertices.size() - meshlet.vertexCount;

        meshlets.push_back(meshlet);
    }

    return meshlets;
}

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
