#pragma once

#include "Model/Structures/AABB.h"
#include "Model/Structures/IndexedTriangle.h"
#include "Model/Structures/Edge.h"
#include "glm/ext/vector_float3.hpp"
#include <array>
#include <vector>

// This is an octal tree which instead of points contains triangles.
struct OcTreeTriangles
{

  public:
    struct Query
    {
        std::vector<IndexedTriangle> triangles = {};

        Query(const std::vector<IndexedTriangle>& triangles) : triangles(triangles)
        {
        }
    };

    OcTreeTriangles() = default;
    OcTreeTriangles(const AABB boundary, const uint32_t capacity);
    OcTreeTriangles(OcTreeTriangles&& other);
    ~OcTreeTriangles();

    void Subdivide();
    bool Push(const IndexedTriangle& triangle);
    std::vector<Query> GetAllNodeTriangles();
	uint32_t CountTriangles(const uint32_t& count = 0) const;

    OcTreeTriangles& operator=(OcTreeTriangles&& other) noexcept;

    static std::vector<Edge> GenerateEdges(const OcTreeTriangles& ocTree, const bool showAllNodes = false);

    uint32_t capacity = 0;
    std::array<OcTreeTriangles*, 8> nodes = {nullptr, nullptr, nullptr, nullptr,
                                             nullptr, nullptr, nullptr, nullptr}; // A, B, C, D, E, F, G, H
    std::vector<IndexedTriangle> triangles = {};

    AABB boundary;
    bool isDivided = false;
};
