#include "OcTree.h"

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
                                   boundary.minPoint + (boundary.maxPoint - boundary.minPoint) * Vec3f(0.5)};

    const Vec3f halfDimensions = halfBoundary.Dimensions();

    const AABB A = {halfBoundary.minPoint, halfBoundary.maxPoint};
    nodes[0] = new OcTreeTriangles(A, capacity);

    const AABB B = {halfBoundary.minPoint + Vec3f(halfDimensions.x, 0, 0),
                    halfBoundary.maxPoint + Vec3f(halfDimensions.x, 0, 0)};
    nodes[1] = new OcTreeTriangles(B, capacity);

    const AABB C = {halfBoundary.minPoint + Vec3f(halfDimensions.x, 0, halfDimensions.z),
                    halfBoundary.maxPoint + Vec3f(halfDimensions.x, 0, halfDimensions.z)};
    nodes[2] = new OcTreeTriangles(C, capacity);

    const AABB D = {halfBoundary.minPoint + Vec3f(0, 0, halfDimensions.z),
                    halfBoundary.maxPoint + Vec3f(0, 0, halfDimensions.z)};
    nodes[3] = new OcTreeTriangles(D, capacity);

    const AABB E = {halfBoundary.minPoint + Vec3f(0, halfDimensions.y, 0),
                    halfBoundary.maxPoint + Vec3f(0, halfDimensions.y, 0)};
    nodes[4] = new OcTreeTriangles(E, capacity);

    const AABB F = {halfBoundary.minPoint + Vec3f(halfDimensions.x, halfDimensions.y, 0),
                    halfBoundary.maxPoint + Vec3f(halfDimensions.x, halfDimensions.y, 0)};
    nodes[5] = new OcTreeTriangles(F, capacity);

    const AABB G = {halfBoundary.minPoint + Vec3f(halfDimensions.x, halfDimensions.y, halfDimensions.z),
                    halfBoundary.maxPoint + Vec3f(halfDimensions.x, halfDimensions.y, halfDimensions.z)};
    nodes[6] = new OcTreeTriangles(G, capacity);

    const AABB H = {halfBoundary.minPoint + Vec3f(0, halfDimensions.y, halfDimensions.z),
                    halfBoundary.maxPoint + Vec3f(0, halfDimensions.y, halfDimensions.z)};
    nodes[7] = new OcTreeTriangles(H, capacity);

    for (const auto& triangle : triangles)
    {

        for (uint8_t i = 0; i < 8; i++)
        {
            if (triangle.Intersects(nodes[i]->boundary))
            {
                nodes[i]->triangles.emplace_back(triangle);
                break;
            }
        }
    }

    triangles.clear();
    isDivided = true;
}

bool OcTreeTriangles::Push(const IndexedTriangle& triangle)
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

    if (triangle.Intersects(boundary))
    {
        triangles.emplace_back(triangle);
        return true;
    }

    return false;
}

void OcTreeTriangles::GetAllNodeTriangles(std::vector<Query>& outQueries)
{
    if (!isDivided)
    {
        outQueries.emplace_back(triangles);
        return;
    }

    for (uint8_t i = 0; i < 8; i++)
    {
        nodes[i]->GetAllNodeTriangles(outQueries);
    }
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
uint32_t OcTreeTriangles::CountTriangles(const uint32_t& count) const {

	uint32_t newCount = triangles.size();

	if (isDivided) {
		for (size_t i = 0; i < 8; i++) {
			newCount += nodes[i]->CountTriangles(newCount);
		}
	}

	return newCount;
}
