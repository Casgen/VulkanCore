#include "OcTree.h"
#include "glm/ext/vector_float3.hpp"

OcTree::OcTree(const AABB boundary, const uint32_t capacity) : boundary(boundary), capacity(capacity), points()
{
    points.reserve(capacity);
}

OcTree::OcTree(OcTree&& other)
{
	nodes = other.nodes;
	isDivided = other.isDivided;
	capacity = other.capacity;
	points = std::move(other.points);
	boundary = other.boundary;

	for (OcTree* node : other.nodes) {
		node = nullptr;
	}

	other.isDivided = false;
	other.points.clear();
}

OcTree& OcTree::operator=(OcTree&& other) noexcept
{
	if (this == &other) {
		return *this;
	}

	nodes = other.nodes;
	isDivided = other.isDivided;
	capacity = other.capacity;
	points = std::move(other.points);
	boundary = other.boundary;

	for (OcTree* node : other.nodes) {
		node = nullptr;
	}

	other.isDivided = false;
	other.points.clear();

	return *this;
}

OcTree::~OcTree()
{
    if (isDivided)
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            delete nodes[i];
        }
    }
}

void OcTree::Subdivide()
{
    const AABB halfBoundary = {.minPoint = boundary.minPoint,
                               .maxPoint =
                                   boundary.minPoint + (boundary.maxPoint - boundary.minPoint) * glm::vec3(0.5)};

    const glm::vec3 halfDimensions = halfBoundary.Dimensions();

    const AABB A = {halfBoundary.minPoint, halfBoundary.maxPoint};
    nodes[0] = new OcTree(A, capacity);

    const AABB B = {halfBoundary.minPoint + glm::vec3(halfDimensions.x, 0, 0),
                    halfBoundary.maxPoint + glm::vec3(halfDimensions.x, 0, 0)};
    nodes[1] = new OcTree(B, capacity);

    const AABB C = {halfBoundary.minPoint + glm::vec3(halfDimensions.x, 0, halfDimensions.z),
                    halfBoundary.maxPoint + glm::vec3(halfDimensions.x, 0, halfDimensions.z)};
    nodes[2] = new OcTree(C, capacity);

    const AABB D = {halfBoundary.minPoint + glm::vec3(0, 0, halfDimensions.z),
                    halfBoundary.maxPoint + glm::vec3(0, 0, halfDimensions.z)};
    nodes[3] = new OcTree(D, capacity);

    const AABB E = {halfBoundary.minPoint + glm::vec3(0, halfDimensions.y, 0),
                    halfBoundary.maxPoint + glm::vec3(0, halfDimensions.y, 0)};
    nodes[4] = new OcTree(E, capacity);

    const AABB F = {halfBoundary.minPoint + glm::vec3(halfDimensions.x, halfDimensions.y, 0),
                    halfBoundary.maxPoint + glm::vec3(halfDimensions.x, halfDimensions.y, 0)};
    nodes[5] = new OcTree(F, capacity);

    const AABB G = {halfBoundary.minPoint + glm::vec3(halfDimensions.x, halfDimensions.y, halfDimensions.z),
                    halfBoundary.maxPoint + glm::vec3(halfDimensions.x, halfDimensions.y, halfDimensions.z)};
    nodes[6] = new OcTree(G, capacity);

    const AABB H = {halfBoundary.minPoint + glm::vec3(0, halfDimensions.y, halfDimensions.z),
                    halfBoundary.maxPoint + glm::vec3(0, halfDimensions.y, halfDimensions.z)};
    nodes[7] = new OcTree(H, capacity);

    for (const auto& point : points)
    {

        for (uint8_t i = 0; i < 8; i++)
        {
            if (nodes[i]->boundary.IsPointInside(point))
            {
                nodes[i]->points.emplace_back(point);
                break;
            }
        }
    }

    points.clear();

    isDivided = true;
}

bool OcTree::Push(const glm::vec3 point)
{
    if (points.size() >= capacity && !isDivided)
    {
        Subdivide();
    }

    if (isDivided)
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            if (nodes[i]->Push(point))
                return true;
        }

        return false;
    }

    if (boundary.IsPointInside(point))
    {
        points.emplace_back(point);
        return true;
    }

    return false;
}

std::vector<OcTree::Query> OcTree::GetAllNodePoints()
{
    std::vector<OcTree::Query> queries = {};

    if (!isDivided)
    {
        queries.emplace_back(points);
        return std::move(queries);
    }

    for (uint8_t i = 0; i < 8; i++)
    {
        std::vector<OcTree::Query> nodePoints = nodes[i]->GetAllNodePoints();
        queries.insert(queries.end(), nodePoints.begin(), nodePoints.end());
    }

    return std::move(queries);
}

std::vector<Edge> OcTree::GenerateEdges(const OcTree& ocTree, const bool showAllNodes)
{

    if ((!ocTree.isDivided && ocTree.points.size() > 0) || showAllNodes)
    {
        return ocTree.boundary.GenerateEdges();
    }

    std::vector<Edge> edges;

    if (ocTree.isDivided)
    {
        for (size_t i = 0; i < 8; i++)
        {
            std::vector<Edge> temp = GenerateEdges(*ocTree.nodes[i]);
            edges.insert(edges.end(), temp.begin(), temp.end());
        }
    }

    return edges;
}

// --------------------------------------------------------------------------------
//
OcTreeTriangles::OcTreeTriangles(const AABB boundary, const uint32_t capacity) : boundary(boundary), capacity(capacity)
{
    triangles.reserve(capacity);
}

OcTreeTriangles::OcTreeTriangles(OcTreeTriangles&& other)
{
	nodes = other.nodes;
	isDivided = other.isDivided;
	capacity = other.capacity;
	triangles = std::move(other.triangles);
	boundary = other.boundary;

	for (OcTreeTriangles* node : other.nodes) {
		node = nullptr;
	}

	other.isDivided = false;
	other.triangles.clear();
}

OcTreeTriangles& OcTreeTriangles::operator=(OcTreeTriangles&& other) noexcept
{
	if (this == &other) {
		return *this;
	}

	nodes = other.nodes;
	isDivided = other.isDivided;
	capacity = other.capacity;
	triangles = std::move(other.triangles);
	boundary = other.boundary;

	for (OcTreeTriangles* node : other.nodes) {
		node = nullptr;
	}

	other.isDivided = false;
	other.triangles.clear();

	return *this;
}

OcTreeTriangles::~OcTreeTriangles()
{
    if (isDivided)
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            delete nodes[i];
        }
    }
}

void OcTreeTriangles::Subdivide()
{
    const AABB halfBoundary = {.minPoint = boundary.minPoint,
                               .maxPoint =
                                   boundary.minPoint + (boundary.maxPoint - boundary.minPoint) * glm::vec3(0.5)};

    const glm::vec3 halfDimensions = halfBoundary.Dimensions();

    const AABB A = {halfBoundary.minPoint, halfBoundary.maxPoint};
    nodes[0] = new OcTreeTriangles(A, capacity);

    const AABB B = {halfBoundary.minPoint + glm::vec3(halfDimensions.x, 0, 0),
                    halfBoundary.maxPoint + glm::vec3(halfDimensions.x, 0, 0)};
    nodes[1] = new OcTreeTriangles(B, capacity);

    const AABB C = {halfBoundary.minPoint + glm::vec3(halfDimensions.x, 0, halfDimensions.z),
                    halfBoundary.maxPoint + glm::vec3(halfDimensions.x, 0, halfDimensions.z)};
    nodes[2] = new OcTreeTriangles(C, capacity);

    const AABB D = {halfBoundary.minPoint + glm::vec3(0, 0, halfDimensions.z),
                    halfBoundary.maxPoint + glm::vec3(0, 0, halfDimensions.z)};
    nodes[3] = new OcTreeTriangles(D, capacity);

    const AABB E = {halfBoundary.minPoint + glm::vec3(0, halfDimensions.y, 0),
                    halfBoundary.maxPoint + glm::vec3(0, halfDimensions.y, 0)};
    nodes[4] = new OcTreeTriangles(E, capacity);

    const AABB F = {halfBoundary.minPoint + glm::vec3(halfDimensions.x, halfDimensions.y, 0),
                    halfBoundary.maxPoint + glm::vec3(halfDimensions.x, halfDimensions.y, 0)};
    nodes[5] = new OcTreeTriangles(F, capacity);

    const AABB G = {halfBoundary.minPoint + glm::vec3(halfDimensions.x, halfDimensions.y, halfDimensions.z),
                    halfBoundary.maxPoint + glm::vec3(halfDimensions.x, halfDimensions.y, halfDimensions.z)};
    nodes[6] = new OcTreeTriangles(G, capacity);

    const AABB H = {halfBoundary.minPoint + glm::vec3(0, halfDimensions.y, halfDimensions.z),
                    halfBoundary.maxPoint + glm::vec3(0, halfDimensions.y, halfDimensions.z)};
    nodes[7] = new OcTreeTriangles(H, capacity);

    for (const auto& triangle : triangles)
    {

        for (uint8_t i = 0; i < 8; i++)
        {
            if (triangle.Overlaps(nodes[i]->boundary))
            {
                nodes[i]->triangles.emplace_back(triangle);
                break;
            }
        }
    }

    triangles.clear();
    isDivided = true;
}

bool OcTreeTriangles::Push(const Triangle& triangle)
{
    if (triangles.size() >= capacity && !isDivided)
    {
        Subdivide();
    }

    if (isDivided)
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            if (nodes[i]->Push(triangle))
                return true;
        }

        return false;
    }

    if (triangle.Overlaps(boundary))
    {
        triangles.emplace_back(triangle);
        return true;
    }

    return false;
}

std::vector<OcTreeTriangles::Query> OcTreeTriangles::GetAllNodeTriangles()
{
    std::vector<OcTreeTriangles::Query> queries = {};

    if (!isDivided)
    {
        queries.emplace_back(triangles);
        return std::move(queries);
    }

    for (uint8_t i = 0; i < 8; i++)
    {
        std::vector<OcTreeTriangles::Query> nodeTriangles = nodes[i]->GetAllNodeTriangles();
        queries.insert(queries.end(), nodeTriangles.begin(), nodeTriangles.end());
    }

    return std::move(queries);
}

std::vector<Edge> OcTreeTriangles::GenerateEdges(const OcTreeTriangles& ocTree, const bool showAllNodes)
{
    std::vector<Edge> edges;

    if ((!ocTree.isDivided && ocTree.triangles.size() > 0) || showAllNodes)
    {
        edges = ocTree.boundary.GenerateEdges();
    }

    if (ocTree.isDivided)
    {
        for (size_t i = 0; i < 8; i++)
        {
            std::vector<Edge> temp = GenerateEdges(*ocTree.nodes[i]);
            edges.insert(edges.end(), temp.begin(), temp.end());
        }
    }

    return edges;
}
