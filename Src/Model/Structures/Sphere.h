#pragma once

#include "Mesh/MeshVertex.h"
#include "Vk/Buffers/Buffer.h"
#include "./../Vendor/ZMath/Vec3f.h"
#include "glm/ext/scalar_constants.hpp"
#include "vulkan/vulkan_enums.hpp"
#include <vector>

struct Sphere
{
    Vec3f center;
    float r = 1.f;

	Sphere() = default;

    Sphere(const Vec3f& center, const float radius) : center(center), r(radius)
    {
    }

    inline bool isPointInside(const Vec3f& point)
    {
        return (point - center).Magnitude() <= r;
    }
};

struct SphereModel : public Sphere
{

    VkCore::Buffer m_IndexBuffer;
    VkCore::Buffer m_Vertexbuffer;

    std::vector<uint32_t> indices;
    std::vector<LineVertex> vertices;

	SphereModel() = default;

    SphereModel(const Vec3f center, const float radius = 1.f, const Vec3f color = {1.f, 0.f, 0.f},
                const uint32_t m = 10, const uint32_t n = 10)
        : Sphere(center, radius)
    {

        indices.reserve(m * 2 * n * 2);
        vertices.reserve(m * n);

        float phiUnit = glm::pi<float>() / (float)(m - 1);
        float thetaUnit = glm::pi<float>() * 2 / (float)(n - 1);

        float texX_div = 1.f / (static_cast<float>(m - 1));
        float texY_div = 1.f / (static_cast<float>(n - 1));

        for (unsigned int i = 0; i < m; i++)
        {
            float phi = phiUnit * (float)i;
            float tex_y = 0.0f + texY_div * static_cast<float>(i);

            for (unsigned int j = 0; j < n; j++)
            {
                float theta = thetaUnit * (float)j;
                float tex_x = 0.0f + texX_div * static_cast<float>(j);

                Vec3f vertexPosition = Vec3f(r * sin(phi) * cos(theta) + center.x, r * sin(phi) * sin(theta) + center.y,
                                             r * cos(phi) + center.z);

				float rad = (vertexPosition - center).Magnitude();

                Vec3f normal = (vertexPosition - Vec3f(center)).Normalize();

                LineVertex vertex = {.Position = glm::vec3(vertexPosition.x, vertexPosition.y, vertexPosition.z),
                                     .Normal = glm::vec3(normal.x, normal.y, normal.z),
                                     .Color = glm::vec3(color.x, color.y, color.z)};

                vertices.push_back(vertex);
            }
        }

        for (unsigned int i = 0; i < m; i++)
        {
            for (unsigned int j = 0; j < n - 1; j++)
            {
                indices.emplace_back(i * n + j);
                indices.emplace_back(i * n + j + 1);
            }
        }

        for (unsigned int i = 0; i < m - 1; i++)
        {
            for (unsigned int j = 0; j < n; j++)
            {
                indices.emplace_back(i * n + j);
                indices.emplace_back((i + 1) * n + j);
            }
        }

        m_IndexBuffer = VkCore::Buffer(vk::BufferUsageFlagBits::eIndexBuffer);
        m_IndexBuffer.InitializeOnGpu(indices.data(), sizeof(uint32_t) * indices.size());

        m_Vertexbuffer = VkCore::Buffer(vk::BufferUsageFlagBits::eVertexBuffer);
        m_Vertexbuffer.InitializeOnGpu(vertices.data(), sizeof(LineVertex) * vertices.size());
    }
};
