#pragma once

#include "Vk/Buffers/Buffer.h"
#include "Vk/Vertex/VertexAttributeBuilder.h"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"

struct MeshVertex
{
    glm::vec3 Position;
    alignas(16) glm::vec3 Normal;
    alignas(16) glm::vec3 Tangent;
    alignas(16) glm::vec3 BiTangent;
    glm::vec2 TexCoords;

};

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec3 Tangent;
    glm::vec3 BiTangent;
    glm::vec2 TexCoords;

    static VkCore::VertexAttributeBuilder CreateAttributeBuilder(const uint32_t binding = 0)
    {
        VkCore::VertexAttributeBuilder attributeBuilder{};

        attributeBuilder.SetBinding(binding)
            .PushAttribute<float>(3)
            .PushAttribute<float>(3)
            .PushAttribute<float>(3)
            .PushAttribute<float>(3)
            .PushAttribute<float>(2);

		return attributeBuilder;
    }
};


struct LineVertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec3 Color;
};
