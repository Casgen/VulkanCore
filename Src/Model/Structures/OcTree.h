#pragma once

#include "Model/Structures/AABB.h"
#include "Model/Structures/Triangle.h"
#include "Model/Structures/Edge.h"
#include "glm/ext/vector_float3.hpp"
#include <array>
#include <vector>

struct OcTree
{

  public:
    struct Query
    {
        std::vector<glm::vec3> points = {};

        Query(const std::vector<glm::vec3>& points) : points(points)
        {
        }
    };

    OcTree() = default;
    OcTree(const AABB boundary, const uint32_t capacity);
    OcTree(OcTree&& other);

    ~OcTree();

    void Subdivide();
    bool Push(const glm::vec3 point);
    std::vector<Query> GetAllNodePoints();

    OcTree& operator=(OcTree&& other) noexcept;

    static std::vector<Edge> GenerateEdges(const OcTree& ocTree, const bool showAllNodes = false);

    uint32_t capacity = 0;
    std::array<OcTree*, 8> nodes = {nullptr, nullptr, nullptr, nullptr,
                                    nullptr, nullptr, nullptr, nullptr}; // A, B, C, D, E, F, G, H
    std::vector<glm::vec3> points = {};

    AABB boundary;
    bool isDivided = false;
};

// This is an octal tree which instead of points contains triangles.
struct OcTreeTriangles
{

  public:
    struct Query
    {
        std::vector<Triangle> triangles = {};

        Query(const std::vector<Triangle>& triangles) : triangles(triangles)
        {
        }
    };

    OcTreeTriangles() = default;
    OcTreeTriangles(const AABB boundary, const uint32_t capacity);
    OcTreeTriangles(OcTreeTriangles&& other);
    ~OcTreeTriangles();

    void Subdivide();
    bool Push(const Triangle& triangle);
    std::vector<Query> GetAllNodeTriangles();

    OcTreeTriangles& operator=(OcTreeTriangles&& other) noexcept;

    static std::vector<Edge> GenerateEdges(const OcTreeTriangles& ocTree, const bool showAllNodes = false);

    uint32_t capacity = 0;
    std::array<OcTreeTriangles*, 8> nodes = {nullptr, nullptr, nullptr, nullptr,
                                             nullptr, nullptr, nullptr, nullptr}; // A, B, C, D, E, F, G, H
    std::vector<Triangle> triangles = {};

    AABB boundary;
    bool isDivided = false;
};
