#pragma once

namespace Constants
{
    constexpr const unsigned int MAX_MESHLET_VERTICES = 64;
    constexpr const unsigned int MAX_MESHLET_INDICES = 384;
    constexpr const unsigned int MAX_MESHLET_TRIANGLES = MAX_MESHLET_INDICES / 3;

	constexpr const unsigned int MAX_LOD_LEVELS = 8;
} // namespace Constants
