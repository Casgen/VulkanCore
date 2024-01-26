#pragma once

#include "Buffer.h"

namespace VkCore
{

    class VertexBuffer : public Buffer
    {

      public:
        VertexBuffer() : Buffer(vk::BufferUsageFlagBits::eVertexBuffer) {
            
        }

    };
} // namespace VkCore
