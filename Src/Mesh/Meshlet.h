#pragma once

#include <cstdint>

struct Meshlet
{
    uint32_t vertices[4];
    uint32_t indices[8];
    uint32_t vertexCount;
    uint32_t indicesCount;
};
