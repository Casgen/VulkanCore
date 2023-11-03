#pragma once

#include "glm/ext/matrix_transform.hpp"
#include "glm/mat4x4.hpp"

class MatrixBuffer
{
  public:
    MatrixBuffer() {}

    glm::mat4 m_Model = glm::mat4(1.f);
    glm::mat4 m_View;
    glm::mat4 m_Proj;

};
