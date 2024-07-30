#pragma once

#include "Model/Camera.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/mat4x4.hpp"

class FrustumMatrixBuffer
{
  public:
    FrustumMatrixBuffer() {}

    glm::mat4 m_Model = glm::mat4(1.f);
    glm::mat4 m_View;
    glm::mat4 m_Proj;
	Frustum frustum;
};

class MatrixBuffer
{
  public:
    MatrixBuffer() {}

    glm::mat4 m_Model = glm::mat4(1.f);
    glm::mat4 m_View;
    glm::mat4 m_Proj;
};
