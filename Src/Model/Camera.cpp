#include "Mesh/MeshVertex.h"
#include "glm/ext/matrix_float4x4.hpp"
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "Camera.h"
#include "Log/Log.h"
#include "glm/common.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/geometric.hpp"
#include <cstdio>

Camera::Camera(const glm::vec3& position, const glm::vec3 lookAt, const float aspectRatio, const float fov, const float far)
    : m_Position(position), m_Fov(45.f), m_AspectRatio(aspectRatio), m_Far(far)
{
    m_ProjectionMat = glm::perspective(fov, aspectRatio, m_Near, m_Far);
    m_FwdVector = -glm::normalize(lookAt - position);

    // The up vector is reversed due to the Vulkan Coordinate system having Y-Axis flipped
    m_UpVector = glm::vec3(0.f, -1.f, 0.f);
    m_SideVector = glm::cross(m_UpVector, m_FwdVector);
    m_ViewMat = glm::lookTo(position, -m_FwdVector, m_UpVector);

	m_Azimuth = (float) -atan2(-m_FwdVector.x, -m_FwdVector.z);
	m_Zenith = acosf(-m_FwdVector.y) - (glm::pi<float>() / 2.f);
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

	if (m_MovementSpeed <= m_MovementSpeedMax && m_MovementSpeed > 0.f ) {
		m_AccelerationInc += value / 4.f;
	}
}

void Camera::AddMovementSpeed(const float value)
{
    m_MovementSpeed = glm::clamp(m_MovementSpeed + value, 0.f, m_MovementSpeedMax);

	if (m_MovementSpeed <= m_MovementSpeedMax && m_MovementSpeed > 0.f ) {
		m_AccelerationInc += value / 4.f;
	}
}

void Camera::SetAzimuth(const float angle)
{
    m_Azimuth = angle;
    UpdateVectors();
}

void Camera::SetPosition(const glm::vec3 position)
{
    m_Position = position;
    m_ViewMat = glm::lookTo(position, -m_FwdVector, m_UpVector);
}

void Camera::SetZenith(const float angle)
{
    this->m_Zenith = glm::clamp(angle, -(glm::pi<float>() / 2), ((glm::pi<float>() - 0.001f) / 2));
    UpdateVectors();

    m_ViewMat = glm::lookTo(m_Position, m_FwdVector, m_UpVector);
}

void Camera::Yaw(const float step)
{
    m_Azimuth = fmod(m_Azimuth + (step) * -m_RotationSpeed, glm::pi<float>() * 2);
    UpdateVectors();
}

void Camera::Pitch(const float step)
{
    float newZenith = (this->m_Zenith + (float)step * m_RotationSpeed);

    this->m_Zenith = glm::clamp(newZenith, -(glm::pi<float>() / 2), ((glm::pi<float>() - 0.001f) / 2));

    UpdateVectors();

    m_ViewMat = glm::lookTo(m_Position, m_FwdVector, m_UpVector);
}

void Camera::UpdateVectors()
{
    m_FwdVector = glm::vec3(sin(m_Azimuth) * cos(m_Zenith), sin(m_Zenith), -cos(m_Azimuth) * cos(m_Zenith));

    // TODO: Has to be updated for plane normals to work!!!!
    m_UpVector =
        -glm::vec3(sin(m_Azimuth) * cos(m_Zenith + glm::pi<float>() / 2.f), sin(m_Zenith + glm::pi<float>() / 2.f),
                   -cos(m_Azimuth) * cos(m_Zenith + glm::pi<float>() / 2.f));

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

Frustum Camera::CalculateFrustum() const
{
    float fovRadians = (m_Fov * (glm::pi<float>() / 180));
    //
    glm::vec3 normalizedSide = glm::normalize(m_SideVector);
    glm::vec3 normalizedUp = glm::normalize(m_UpVector);
    glm::vec3 normalizedFwd = glm::normalize(-m_FwdVector);

    const float halfVSide = m_Far * tanf(m_Fov * 0.5f);
    const float halfHSide = halfVSide * m_AspectRatio;

    const glm::vec3 farFwdVec = m_Far * normalizedFwd;

    glm::vec3 leftPlaneNormal = glm::normalize(glm::cross(farFwdVec - m_SideVector * halfHSide, -m_UpVector));
    glm::vec3 rightPlaneNormal = glm::normalize(glm::cross(-m_UpVector, farFwdVec + m_SideVector * halfHSide));

    glm::vec3 bottomPlaneNormal = glm::normalize(glm::cross(m_SideVector, farFwdVec - (-m_UpVector) * halfVSide));
    glm::vec3 topPlaneNormal = glm::normalize(glm::cross(farFwdVec + (-m_UpVector) * halfVSide, m_SideVector));

    return {.left = leftPlaneNormal,
            .right = rightPlaneNormal,
            .top = topPlaneNormal,
            .bottom = bottomPlaneNormal,
            .front = -normalizedFwd,
            .back = normalizedFwd,
            .pointSides = m_Position,
            .pointFront = m_Position + (normalizedFwd * m_Near),
            .pointBack = m_Position + (normalizedFwd * m_Far),
			.sideVec = m_SideVector,
            .azimuth = m_Azimuth,
            .zenith = m_Zenith};
}

std::tuple<VkCore::Buffer, VkCore::Buffer> Camera::ConstructFrustumModel() const
{
    glm::vec3 oppositeFwd = glm::vec3(0.f, 0.f, 1.f);
	glm::vec3 upVector = glm::vec3(0.f, -1.f, 0.f);
	glm::vec3 sideVector = glm::vec3(1.f, 0.f, 0.f);

    float fovX = tan(m_Fov * glm::pi<float>() / 180);
    float fovY = fovX / m_AspectRatio;

    float nearHalfHeight = fovY * m_Near;
    float nearHalfWidth = fovX * m_Near;

    float farHalfHeight = fovY * m_Far;
    float farHalfWidth = fovX * m_Far;

	glm::vec3 nearVector = oppositeFwd * m_Near;
	glm::vec3 farVector = oppositeFwd * m_Far;

    std::vector<LineVertex> vertices;
	vertices.resize(9);

    // Set the position first.
    vertices[0] =
        {.Position = glm::vec3(0.f), .Normal = glm::vec3(), .Color = glm::vec3(1.0f / 0x89, 1.0f / 0x89, 1.0f / 0x89)};

    // Add all the other frustum vertices.
    vertices[1] = {.Position = nearVector - (upVector * nearHalfHeight) - (sideVector * nearHalfWidth), .Normal = glm::vec3(), .Color = glm::vec3(0x89 / 255.0)};
    vertices[2] = {.Position = nearVector + (upVector * nearHalfHeight) - (sideVector * nearHalfWidth), .Normal = glm::vec3(), .Color = glm::vec3(0x89 / 255.0)};
    vertices[3] = {.Position = nearVector + (upVector * nearHalfHeight) + (sideVector * nearHalfWidth), .Normal = glm::vec3(), .Color = glm::vec3(0x89 / 255.0)};
    vertices[4] = {.Position = nearVector - (upVector * nearHalfHeight) + (sideVector * nearHalfWidth), .Normal = glm::vec3(), .Color = glm::vec3(0x89 / 255.0)};

    vertices[5] = {.Position = farVector - (upVector * farHalfHeight) - (sideVector * farHalfWidth), .Normal = glm::vec3(), .Color = glm::vec3(0xE3 / 255.0)};
    vertices[6] = {.Position = farVector + (upVector * farHalfHeight) - (sideVector * farHalfWidth), .Normal = glm::vec3(), .Color = glm::vec3(0xE3 / 255.0)};
    vertices[7] = {.Position = farVector + (upVector * farHalfHeight) + (sideVector * farHalfWidth), .Normal = glm::vec3(), .Color = glm::vec3(0xE3 / 255.0)};
    vertices[8] = {.Position = farVector - (upVector * farHalfHeight) + (sideVector * farHalfWidth), .Normal = glm::vec3(), .Color = glm::vec3(0xE3 / 255.0)};

    std::vector<uint32_t> indices = {0, 1, 0, 2, 0, 3, 0, 4, 1, 2, 2, 3, 3, 4, 4, 1,
                                     1, 5, 2, 6, 3, 7, 4, 8, 5, 6, 6, 7, 7, 8, 8, 5};

    VkCore::Buffer indexBuffer = VkCore::Buffer(vk::BufferUsageFlagBits::eIndexBuffer);
    indexBuffer.InitializeOnGpu(indices.data(), indices.size() * sizeof(uint32_t));

    VkCore::Buffer vertexBuffer = VkCore::Buffer(vk::BufferUsageFlagBits::eVertexBuffer);
    vertexBuffer.InitializeOnGpu(vertices.data(), vertices.size() * sizeof(LineVertex));

    return {std::move(vertexBuffer), std::move(indexBuffer)};
}
