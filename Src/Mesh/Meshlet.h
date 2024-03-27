#pragma once

#include <cstdint>
#include "../Constants.h"

struct Meshlet
{
    uint32_t indices[Constants::MAX_MESHLET_INDICES];
    uint32_t vertices[Constants::MAX_MESHLET_VERTICES];
    uint32_t vertexCount;
    uint32_t indicesCount;
};
