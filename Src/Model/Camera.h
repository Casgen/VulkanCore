#pragma once

#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

/**
 *  A struct with a bit field to identify in which direction is the camera moving.
 */
struct MovingBitField
{

  public:
    bool isFwd : 1, isLeft : 1, isBack : 1, isRight : 1, isUp : 1, isDown : 1;

    /**
     * Constructs a bit field;
     * @param isFree - Sets a Free mode boolean to indicate that the camera can freely move.
     */
    MovingBitField() : isFwd(false), isLeft(false), isBack(false), isRight(false), isUp(false), isDown(false)
    {
    }

    bool IsMoving()
    {
        return isFwd || isLeft || isBack || isRight || isUp || isDown;
    }
};

class Camera
{

  public:
    Camera() = default;
    Camera(const glm::vec3& position, const glm::vec3 lookAt, const float aspectRatio);

    // Updates the state of the camera
    void Update();

    // ---------------- Camera Movement ----------------
    void SetIsMovingRight(const bool value);
    void SetIsMovingLeft(const bool value);
    void SetIsMovingUp(const bool value);
    void SetIsMovingDown(const bool value);
    void SetIsMovingForward(const bool value);
    void SetIsMovingBackward(const bool value);

    // Sets the movement speed of the camera
    void SetMovementSpeed(const float value);

    /** 
     * adds the movement speed by the given value. The value is also being clamped to the Max and min movement IncMovementSpeed
     */
    void AddMovementSpeed(const float value);

    // ---------------- Camera Rotation ----------------
    void Yaw(const float step);
    void Pitch(const float step);

    void RecreateProjection(const int width, const int height);

    // ------------------ Getters ----------------------

    float GetMovementSpeed() const;

    bool GetIsMovingRight() const;
    bool GetIsMovingLeft() const;
    bool GetIsMovingUp() const;
    bool GetIsMovingDown() const;
    bool GetIsMovingForward() const;
    bool GetIsMovingBackward() const;

    [[nodiscard]] glm::mat4 GetViewMatrix()
    {
        return m_ViewMat;
    }
    [[nodiscard]] glm::mat4 GetProjMatrix()
    {
        return m_ProjectionMat;
    }

  private:
    // Vectors of a Camera
    glm::vec3 m_Position;
    glm::vec3 m_FwdVector, m_SideVector, m_UpVector;

    // Camera Matrices
    glm::mat4 m_ViewMat;
    glm::mat4 m_ProjectionMat;

    // Movement member variables
    float m_MovementSpeed = .01f;
    float m_MovementSpeedMax = .75f;
    float m_CurrAcceleration = 0.f;
    float m_AccelerationInc = .0005f;
    float m_RotationSpeed = .01f;

    // Camera Parameters
    float m_AspectRatio = 0.f;
    float m_Azimuth, m_Zenith = 0.f;
    glm::vec3 m_CurrentMovingDir = glm::vec3(0.f);
    MovingBitField m_MovingBitField{};

    void UpdateVectors();
};
