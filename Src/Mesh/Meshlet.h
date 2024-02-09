#pragma once

#include <cstdint>

struct Meshlet
{
    uint32_t vertices[64];
    uint32_t indices[378];
    uint32_t vertexCount;
    uint32_t indicesCount;
};
