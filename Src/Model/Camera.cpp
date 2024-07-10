#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "Camera.h"
#include "Log/Log.h"
#include "glm/common.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"
#include <cstdio>

Camera::Camera(const glm::vec3& position, const glm::vec3 lookAt, const float aspectRatio, const float fov)
    : m_Position(position), m_Fov(45.f), m_AspectRatio(aspectRatio)
{
    m_ProjectionMat = glm::perspective(fov, aspectRatio, .01f, 5.f);
    m_FwdVector = -glm::normalize(lookAt - position);

    // The up vector is reversed due to the Vulkan Coordinate system having Y-Axis flipped
    m_UpVector = glm::vec3(0.f, -1.f, 0.f);
    m_SideVector = glm::cross(m_UpVector, m_FwdVector);
    m_ViewMat = glm::lookTo(position, -m_FwdVector, m_UpVector);
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

        m_CurrentMovingDir += m_FwdVector * (float)m_MovingBitField.isFwd;  // For Forward movement
        m_CurrentMovingDir -= m_FwdVector * (float)m_MovingBitField.isBack; // For Backward movement
    }
    else
    {
        m_CurrAcceleration = glm::clamp(m_CurrAcceleration - m_AccelerationInc, 0.0f, m_MovementSpeed);
    }

    m_Position += -m_CurrentMovingDir * m_CurrAcceleration;
    m_ViewMat = glm::lookTo(m_Position, -m_FwdVector, m_UpVector);
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
    m_Azimuth = fmod(m_Azimuth + (step) * -m_RotationSpeed, glm::pi<float>() * 2);
    UpdateVectors();
}

void Camera::Pitch(const float step)
{
    float newZenith = (this->m_Zenith + (float)step * m_RotationSpeed);

    this->m_Zenith = glm::clamp(newZenith, -(glm::pi<float>() / 2), (glm::pi<float>() / 2));
    UpdateVectors();
    m_ViewMat = glm::lookTo(m_Position, m_FwdVector, m_UpVector);
}

void Camera::UpdateVectors()
{
    m_FwdVector = glm::vec3(sin(this->m_Azimuth) * cos(this->m_Zenith), sin(this->m_Zenith),
                            -cos(this->m_Azimuth) * cos(this->m_Zenith));

    m_UpVector = glm::vec3(0, -1.f, 0.f);

    m_SideVector = glm::cross(m_UpVector, m_FwdVector);
}

void Camera::RecreateProjection(const int width, const int height)
{
    m_AspectRatio = width / static_cast<float>(height);

    m_ProjectionMat = glm::perspective(45.0f, m_AspectRatio, 0.001f, 50.f);
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

Frustum Camera::CalculateFrustumNormals() const
{
    float fovRadians = (m_Fov * (glm::pi<float>() / 180));

    glm::vec3 normalizedSide = glm::normalize(m_SideVector);
    glm::vec3 normalizedUp = glm::normalize(m_UpVector);
    glm::vec3 normalizedFwd = glm::normalize(m_FwdVector);

    // axis-angle rotation
    glm::vec3 rightPlaneNormal = normalizedSide * cosf(-fovRadians) +
                                 glm::dot(normalizedSide, normalizedUp) * normalizedUp * (1 - cosf(-fovRadians)) +
                                 glm::cross(normalizedUp, normalizedSide) * sinf(-fovRadians);

    glm::vec3 leftPlaneNormal = -normalizedSide * cosf(fovRadians) +
                                glm::dot(-normalizedSide, normalizedUp) * normalizedUp * (1 - cosf(fovRadians)) +
                                glm::cross(normalizedUp, -normalizedSide) * sinf(fovRadians);

    glm::vec3 topPlaneNormal = normalizedFwd * cosf(-fovRadians) +
                               glm::dot(normalizedFwd, normalizedSide) * normalizedSide * (1 - cosf(-fovRadians)) +
                               glm::cross(normalizedFwd, normalizedSide) * sinf(-fovRadians);

    glm::vec3 bottomPlaneNormal = normalizedFwd * cosf(fovRadians) +
                                  glm::dot(normalizedFwd, normalizedSide) * normalizedSide * (1 - cosf(fovRadians)) +
                                  glm::cross(normalizedFwd, normalizedSide) * sinf(fovRadians);

    return {
        .left = leftPlaneNormal,
        .right = rightPlaneNormal,
        .top = topPlaneNormal,
        .bottom = bottomPlaneNormal,
        .front = -normalizedFwd,
        .back = normalizedFwd,
        .pointSides = m_Position,
        .pointFront = m_Position + (-normalizedFwd * .01f),
        .pointBack = m_Position + (-normalizedFwd * 5.f),
    };
}
