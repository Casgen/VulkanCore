
#include "MeshletGeneration.h"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <immintrin.h>
#include <xmmintrin.h>
#include "../Log/Log.h"

#include "Constants.h"
#include "Log/Log.h"
#include "Mesh/Meshlet.h"
#include "Model/Structures/OcTree.h"
#include "Model/Structures/IndexedTriangle.h"
#include "Model/Structures/Stack.h"
#include "glm/common.hpp"
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
            meshlet.triangleOffset = outIndices.size() - meshlet.triangleCount * 3;
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

        outIndices.emplace_back(av);
        outIndices.emplace_back(bv);
        outIndices.emplace_back(cv);
		
		meshlet.triangleCount++;
    }

    if (meshlet.triangleCount != 0)
    {
		meshlet.triangleOffset = outIndices.size() - meshlet.triangleCount * 3;
		meshlet.vertexOffset = outVertices.size() - meshlet.vertexCount;

        meshlets.push_back(meshlet);
    }

 //    std::vector<uint32_t> packedMeshletTriangles;
 //    packedMeshletTriangles.reserve(outIndices.size() / 3);
	//
	//
 //    for (auto& meshlet : meshlets)
 //    {
	//
	// 	const uint32_t indexOffset = meshlet.triangleOffset * 3;
	//
 //        for (int i = indexOffset; i < indexOffset + meshlet.triangleCount * 3; i += 3)
 //        {
 //            uint32_t packedTriangle = (outIndices.at(i) & 0xFF);
 //            packedTriangle |= (outIndices.at(i + 1) & 0xFF ) << 8;
 //            packedTriangle |= (outIndices.at(i + 2) & 0xFF ) << 16;
	//
 //            packedMeshletTriangles.emplace_back(packedTriangle);
 //        }
 //    }
	//
	// outIndices = packedMeshletTriangles;


    // ---

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

std::vector<Meshlet> MeshletGeneration::OcTreeMeshletizeMesh(uint32_t maxVerts, uint32_t maxIndices, const Mesh& mesh)
{

    std::vector<IndexedTriangle> triangles;
    triangles.reserve(mesh.indices.size() / 3);

    if (mesh.indices.size() % 3 != 0)
    {
        LOGF(Rendering, Warning,
             "The number of indices in the model do not look like triangles! Mesh may not be complete! Indices count: "
             "%d",
             mesh.indices.size())
    }

    for (size_t i = 0; i < mesh.indices.size() - 3; i += 3)
    {

        const uint32_t a = mesh.indices[i];
        const uint32_t b = mesh.indices[i + 1];
        const uint32_t c = mesh.indices[i + 2];

        triangles.emplace_back(mesh.vertices[a].Position, mesh.vertices[b].Position, mesh.vertices[c].Position, a, b,
                               c);
    }

    OcTreeTriangles ocTree = OcTreeTriangles(Mesh::CreateBoundingBox(mesh), Constants::MAX_MESHLET_INDICES / 3);

    for (const auto& triangle : triangles)
    {
        ocTree.Push(triangle);
    }

    std::vector<OcTreeTriangles::Query> queries;
    ocTree.GetAllNodeTriangles(queries);

    std::vector<Meshlet> meshlets;

    for (const auto& query : queries)
    {
        Meshlet meshlet = {};

        std::vector<uint8_t> vertexLookup(mesh.vertices.size(), 0xFF);

        uint8_t vertexCount = 0;

        for (const auto& triangle : query.triangles)
        {

            uint32_t a = mesh.indices[triangle.indexA];
            uint32_t b = mesh.indices[triangle.indexB];
            uint32_t c = mesh.indices[triangle.indexC];

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
std::vector<NewMeshlet> MeshletGeneration::TipsifyAndMeshlet(uint32_t maxVerts, uint32_t maxIndices,
                                                             const std::vector<uint32_t>& indices,
                                                             const uint32_t verticesCount)
{
    // Tipsify the mesh.
    std::vector<uint32_t> tipsifiedIndices = MeshletGeneration::Tipsify(indices, verticesCount, maxVerts);
}

std::vector<uint32_t> MeshletGeneration::Tipsify(const std::vector<uint32_t>& indices, const uint32_t vertexCount,
                                                 const uint32_t cacheSize)
{
    VertexTriangleAdjacency adj = MeshUtils::BuildVertexTriangleAdjecency(indices, vertexCount);

    std::vector<uint32_t> liveTriangles(adj.vertexCount);

    std::vector<uint32_t> cachingTimeStamps(vertexCount);

    std::vector<uint32_t> deadEndStack;

    std::vector<bool> emmitedTriangles(indices.size() / 3);

    int f = 0;

    uint32_t timeStamp = cacheSize + 1;
    uint32_t cursor = 1;

    std::vector<uint32_t> outputIndices;

    while (f >= 0)
    {
        std::vector<uint32_t> ringCandidates;

        std::vector<VertexTriangleAdjacency::Triangle> triangles = adj.GetTriangles(f);
        std::vector<uint32_t> triangleIndices = adj.GetTriangleIndices(f);

        for (size_t i = 0; i < triangles.size(); i++)
        {
            if (!emmitedTriangles[triangleIndices[i]])
            {
                for (size_t t = 0; t < 3; t++)
                {
                    const uint32_t v = triangles[i].vertices[t];
                    outputIndices.emplace_back(v);
                    deadEndStack.emplace_back(v);
                    ringCandidates.emplace_back(v);

                    liveTriangles[v]--;

                    if (timeStamp - cachingTimeStamps[v] > cacheSize)
                    {
                        cachingTimeStamps[v] = ++timeStamp;
                    }
                }
                emmitedTriangles[triangleIndices[i]] = true;
            }
        }

        // Get next fanning vertex ----
        f = MeshletGeneration::GetNextVertex(vertexCount, cursor, cacheSize, ringCandidates, cachingTimeStamps,
                                             timeStamp, liveTriangles, deadEndStack);
    }

    return outputIndices;
}

int MeshletGeneration::GetNextVertex(const uint32_t vertexCount, uint32_t& i, const uint32_t cacheSize,
                                     const std::vector<uint32_t>& candidates, const std::vector<uint32_t>& timeStamps,
                                     uint32_t& timeStamp, const std::vector<uint32_t>& liveTriangles,
                                     std::vector<uint32_t>& stack)
{
    int bestCandidate = -1;
    int priority = -1;
    int prevPriority = -1;

    for (size_t v = 0; v < candidates.size(); v++)
    {
        if (liveTriangles[v] > 0)
        {

            const uint32_t candidate = candidates[v];
            priority = 0;

            if (timeStamp - candidate + 2 * liveTriangles[v] <= cacheSize)
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
        bestCandidate = MeshletGeneration::SkipDeadEnd(liveTriangles, stack, vertexCount, i);
    }

    return bestCandidate;
}

int MeshletGeneration::SkipDeadEnd(const std::vector<uint32_t>& liveTriangles, std::vector<uint32_t>& stack,
                                   const uint32_t vertexCount, uint32_t& i)
{
    while (!stack.empty())
    {

        const uint32_t d = stack.back();
        stack.pop_back();

        if (liveTriangles[d] > 0)
        {
            return d;
        }
    }

    while (i < vertexCount)
    {
        if (liveTriangles[i] > 0)
        {
            return i;
        }

        i++;
    }

    return -1;
}
