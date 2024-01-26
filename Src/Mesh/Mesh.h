#pragma once

#include "../Vk/Buffers/VertexBuffer.h"

template<typename VertexType>
class Mesh
{

  public:
    Mesh();

  private:
        std::vector<unsigned int> m_Indices;
        std::vector<VertexType> m_vertices;
        VkCore::VertexBuffer m_VertexBuffer;

};
