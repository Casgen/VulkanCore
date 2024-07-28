
#include "MeshletGeneration.h"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <immintrin.h>
#include <limits>
#include <xmmintrin.h>

#include "Mesh/MeshVertex.h"
#include "Mesh/Meshlet.h"
#include "MeshUtils.h"

std::vector<NewMeshlet> MeshletGeneration::MeshletizeNv(uint32_t maxVerts, uint32_t maxIndices,
                                                        const std::vector<uint32_t>& indices,
                                                        const uint32_t verticesSize, std::vector<uint32_t>& outVertices,
                                                        std::vector<uint32_t>& outIndices, const uint32_t vertexOffset,
                                                        const uint32_t triangleOffset)
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
            meshlet.triangleOffset = (outIndices.size() - meshlet.triangleCount) + triangleOffset;
            meshlet.vertexOffset = (outVertices.size() - meshlet.vertexCount) + vertexOffset;

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

        // --- Since GLSL doesn't support 8-bit integers we are packing triangles into a uint.
        outIndices.emplace_back(MeshUtils::PackTriangleIntoUInt(av, bv, cv));

        meshlet.triangleCount++;
    }

    if (meshlet.triangleCount != 0)
    {
        meshlet.triangleOffset = (outIndices.size() - meshlet.triangleCount) + triangleOffset;
        meshlet.vertexOffset = (outVertices.size() - meshlet.vertexCount) + vertexOffset;

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
std::vector<MeshletBounds> MeshletGeneration::ComputeMeshletBounds(const std::vector<MeshVertex>& meshVertices,
                                                                   const std::vector<uint32_t>& meshletVertices,
                                                                   const std::vector<NewMeshlet>& meshlets)
{
    std::vector<MeshletBounds> meshletBounds;
    meshletBounds.reserve(meshlets.size());

    {
        uint accOffset = 0;

        for (auto& meshlet : meshlets)
        {

            // Compute avg normal and cone
            std::vector<Vec3f> normals, vertices;
            normals.reserve(meshlet.vertexCount);
            vertices.reserve(meshlet.vertexCount);

            Vec3f maxPoint = Vec3f(std::numeric_limits<float>::min());
            Vec3f minPoint = Vec3f(std::numeric_limits<float>::max());

            uint32_t maxOffset = meshlet.vertexOffset + meshlet.vertexCount;

            for (uint32_t i = meshlet.vertexOffset; i < maxOffset; i++)
            {
				if (i >= meshletVertices.size()) {
					LOG(Rendering, Error, "Fuck!")
				}

                const glm::vec3& glmNormal = meshVertices[meshletVertices[i]].Normal;
                const glm::vec3& glmPosition = meshVertices[meshletVertices[i]].Position;

                normals.emplace_back(Vec3f(glmNormal.x, glmNormal.y, glmNormal.z));
                vertices.emplace_back(Vec3f(glmPosition.x, glmPosition.y, glmPosition.z));

                maxPoint = Vec3f::Max(maxPoint, Vec3f(glmPosition.x, glmPosition.y, glmPosition.z));
                minPoint = Vec3f::Min(minPoint, Vec3f(glmPosition.x, glmPosition.y, glmPosition.z));
            }

            assert(meshlet.vertexCount == normals.size());

            Vec3f avgNormal;

            for (const auto& normal : normals)
            {
                avgNormal += normal;
            }

            avgNormal /= normals.size();
            avgNormal = avgNormal.Normalize();

            Vec3f coneNormal = avgNormal;
            float minDot = 1.f;

            for (const auto& normal : normals)
            {
                float dot = avgNormal.Dot(normal);

                if (dot < minDot)
                {
                    minDot = dot;
                    coneNormal = normal;
                }
            }

            // We have to account for the fact that a triangle is visible to it's entire hemisphere.
            // Therefore we need to add a 90 degree angle to the the most diverging normal.
            // Could there exist an edge case base the normal would go beyond the triangle hemisphere? probably not.
            float newMinDot = cos(3.141589 / 2 + acosf(minDot));

            // Compute Bounding sphere.

            // Sphere boundingSphere = CreateBoundingSphere(vertices);

            Vec3f sphereCenter = (maxPoint + minPoint) / 2.0;
            float sphereRadius = (sphereCenter - maxPoint).Magnitude();


            meshletBounds.emplace_back(MeshletBounds{
                .normal = {avgNormal.x, avgNormal.y, avgNormal.z},
                .coneAngle = minDot,
                .spherePos = {sphereCenter.x, sphereCenter.y, sphereCenter.z},
                .sphereRadius = sphereRadius,
            });
        }
    }

    ASSERT(meshletBounds.size() == meshlets.size(), "Meshlet bounds and meshlets vectors don't have the same size!");

    return meshletBounds;
}

Sphere MeshletGeneration::CreateBoundingSphere(const std::vector<Vec3f> points)
{
    Vec3f xmax = std::numeric_limits<float>::min();
    Vec3f xmin = std::numeric_limits<float>::max();

    Vec3f ymax = std::numeric_limits<float>::min();
    Vec3f ymin = std::numeric_limits<float>::max();

    Vec3f zmax = std::numeric_limits<float>::min();
    Vec3f zmin = std::numeric_limits<float>::max();

    for (int i = 0; i < points.size(); i++)
    {

        const Vec3f& p = points[i];

        // --- X
        if (p.x > xmax.x)
        {
            xmax = p;
        }

        if (p.x < xmin.x)
        {
            xmin = p;
        }

        // --- Y
        if (p.y > xmax.y)
        {
            ymax = p;
        }

        if (p.y < ymin.y)
        {
            ymin = p;
        }

        // --- Z
        if (p.z > zmax.z)
        {
            zmax = p;
        }

        if (p.z < zmin.z)
        {
            zmin = p;
        }
    }

    const float xSpan = (xmax - xmin).MagnitudeSquared();
    const float ySpan = (ymax - ymin).MagnitudeSquared();
    const float zSpan = (zmax - zmin).MagnitudeSquared();

    Vec3f dia1 = xmin;
    Vec3f dia2 = xmax;
    float maxSpan = xSpan;

    if (ySpan > maxSpan)
    {
        maxSpan = ySpan;
        dia1 = ymin;
        dia2 = ymax;
    }

    if (zSpan > maxSpan)
    {
        maxSpan = zSpan;
        dia1 = zmin;
        dia2 = zmax;
    }

    // Dia1 and Dia2 will create the initial sphere.

    Vec3f sphereCenter = (dia1 + dia2) / 2.f;
    float radiusSq = (dia2 - sphereCenter).MagnitudeSquared();
    float radius = (dia2 - sphereCenter).Magnitude();

    // Second Pass: Increment the current sphere;

    float oldToPSq = 0.f;
    float oldToP = 0.f;
    float oldToNew = 0.f;

    for (uint i = 0; i < points.size(); i++)
    {

        const Vec3f& p = points[i];

        Vec3f diff = p - sphereCenter;

        oldToPSq = diff.MagnitudeSquared();

        if (oldToPSq > radiusSq)
        {
            oldToP = sqrtf(oldToPSq);
            radius = (radius + oldToP) / 2.0;
            radiusSq = radius * radius;
            oldToNew = oldToP - radius;

            sphereCenter = (sphereCenter * radius + p * oldToNew) / oldToP;
        }
    }

    return Sphere(sphereCenter, radius);
}
