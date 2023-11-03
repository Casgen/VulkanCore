#include "Camera.h"
#include "glm/common.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/geometric.hpp"

Camera::Camera(const glm::vec3& position, const glm::vec3 lookAt, const float aspectRatio)
    : m_Position(position), m_AspectRatio(aspectRatio)
{
    m_ProjectionMat = glm::perspective(45.0f, aspectRatio, .001f, 50.f);
    m_BackVector = glm::normalize(position - lookAt);
    m_UpVector = glm::vec3(0.f, -1.f, 0.f);
    m_SideVector = glm::cross(m_UpVector, m_BackVector);
    m_ViewMat = glm::lookAt(position, lookAt, m_UpVector);
}

void Camera::Update()
{
    if (m_MovingBitField.IsMoving())
    {
        m_CurrAcceleration = glm::clamp(m_CurrAcceleration + m_AccelerationInc, 0.f, m_MovementSpeed);

        m_CurrentMovingDir = glm::vec3(0.f);

        m_CurrentMovingDir -= m_SideVector * (float)m_MovingBitField.isRight; // For Right Movement
        m_CurrentMovingDir += m_SideVector * (float)m_MovingBitField.isLeft;  // For Left Movement

        m_CurrentMovingDir += glm::vec3(0.f, 1.f, 0.f) * (float)m_MovingBitField.isUp;   // For Upward movement
        m_CurrentMovingDir -= glm::vec3(0.f, 1.f, 0.f) * (float)m_MovingBitField.isDown; // For Downward movement

        m_CurrentMovingDir += m_BackVector * (float)m_MovingBitField.isFwd;  // For Forward movement
        m_CurrentMovingDir -= m_BackVector * (float)m_MovingBitField.isBack; // For Backward movement
    }
    else
    {
        m_CurrAcceleration = glm::clamp(m_CurrAcceleration - m_AccelerationInc, 0.0f, m_MovementSpeed);
    }

    m_Position += -m_CurrentMovingDir * m_CurrAcceleration;
    m_ViewMat = glm::lookTo(m_Position, -m_BackVector, m_UpVector);
    UpdateVectors();
}

void Camera::SetIsMovingRight(const bool value)
{
    m_MovingBitField.isRight = value;
}

void Camera::SetIsMovingLeft(const bool value)
{
    m_MovingBitField.isLeft = value;
}

void Camera::SetIsMovingUp(const bool value)
{
    m_MovingBitField.isUp = value;
}

void Camera::SetIsMovingDown(const bool value)
{
    m_MovingBitField.isDown = value;
}

void Camera::SetIsMovingForward(const bool value)
{
    m_MovingBitField.isFwd = value;
}

void Camera::SetIsMovingBackward(const bool value)
{
    m_MovingBitField.isBack = value;
}

void Camera::SetMovementSpeed(const float value)
{
    m_MovementSpeed = glm::clamp(value, 0.f, m_MovementSpeedMax);
}

void Camera::AddMovementSpeed(const float value)
{
    m_MovementSpeed = glm::clamp(m_MovementSpeed + value, 0.f, m_MovementSpeedMax);
}

void Camera::Yaw(const float step)
{
    m_Azimuth = m_Azimuth + (-step) * m_RotationSpeed;
    UpdateVectors();
}

void Camera::Pitch(const float step)
{
    float newZenith = (this->m_Zenith + (float)step * m_RotationSpeed);

    this->m_Zenith = glm::clamp(newZenith, -(glm::pi<float>() / 2), (glm::pi<float>() / 2));
    UpdateVectors();
    m_ViewMat = glm::lookTo(m_Position, m_BackVector, m_UpVector);
}

void Camera::UpdateVectors()
{
    float x = cos(this->m_Azimuth) * cos(this->m_Zenith);
    float y = sin(this->m_Azimuth) * cos(this->m_Zenith);
    float z = sin(this->m_Zenith);

    m_BackVector = glm::vec3(sin(this->m_Azimuth) * cos(this->m_Zenith), sin(this->m_Zenith),
                             -cos(this->m_Azimuth) * cos(this->m_Zenith));

    m_UpVector = glm::vec3(0,-1.f,0.f);

    m_SideVector = glm::cross(m_UpVector, m_BackVector);
}

void Camera::RecreateProjection(const int width, const int height)
{
    m_AspectRatio = width / static_cast<float>(height);

    m_ProjectionMat = glm::perspective(45.0f, m_AspectRatio, 0.001f, 100.f);
    return;
}

float Camera::GetMovementSpeed() const
{
    return m_MovementSpeed;
}

bool Camera::GetIsMovingRight() const
{
    return m_MovingBitField.isRight;
}

bool Camera::GetIsMovingLeft() const
{
    return m_MovingBitField.isLeft;
}

bool Camera::GetIsMovingUp() const
{
    return m_MovingBitField.isUp;
}

bool Camera::GetIsMovingDown() const
{
    return m_MovingBitField.isDown;
}

bool Camera::GetIsMovingForward() const
{
    return m_MovingBitField.isFwd;
}

bool Camera::GetIsMovingBackward() const
{
    return m_MovingBitField.isBack;
}
