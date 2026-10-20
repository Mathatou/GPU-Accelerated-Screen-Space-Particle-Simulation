#include "camera.hpp"

/// @brief Implements a simple camera system for 3D navigation and view matrix generation.
Camera::Camera()
{
    position = glm::vec3(0.0f, 0.0f, 5.0f);
    front = glm::vec3(0.0f, 0.0f, -1.0f);
    up = glm::vec3(0.0f, 1.0f, 0.0f);
    right = glm::cross(front, up);
    worldUp = up;
    yaw = glm::radians(-90.0f);
    pitch = glm::radians(0.0f);
    speed = 2.5f;
    sensitivity = 0.001f;
}

/// @brief Updates the camera's front, right, and up vectors based on the current yaw and pitch angles.
void Camera::Update()
{
    glm::vec3 newFront;
    newFront.x = cos(yaw) * cos(pitch);
    newFront.y = sin(pitch);
    newFront.z = sin(yaw) * cos(pitch);
    front = glm::normalize(newFront);
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

/// @brief Generates the view matrix based on the camera's position and orientation, which can be used in the rendering pipeline. 
glm::mat4 Camera::GetViewMatrix(glm::mat4& view)
{
    view = glm::lookAt(position, position + front, up);
    // You can use this view matrix in your rendering pipeline
    return view;
}

/// @brief Handles mouse movement input to adjust the camera's yaw and pitch angles, which in turn updates the camera's front vector. 
void Camera::OnMouseMove(float xoffset, float yoffset)
{
    std::cout << "Mouse moved: xoffset = " << xoffset << ", yoffset = " << yoffset << std::endl;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch -= yoffset;

    front = glm::normalize(glm::vec3(
        cos(yaw) * cos(pitch),
        sin(pitch),
        sin(yaw) * cos(pitch)
    ));
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
    if (pitch > glm::radians(89.0f))
        pitch = glm::radians(89.0f);
    if (pitch < glm::radians(-89.0f))
        pitch = glm::radians(-89.0f);
}

/// @brief Handles mouse scroll input to adjust the camera's movement speed. 
void Camera::OnMouseScroll(float yoffset)
{
    speed += yoffset * 0.1f;
    if (speed < 0.1f)
        speed = 0.1f;
    if (speed > 10.0f)
        speed = 10.0f;
    std::cout << "Camera speed: " << speed << std::endl;
}

/// @brief Handles keyboard input to move the camera in the scene based on the pressed key and elapsed time. 
void Camera::OnKeyboardInput(int key, float deltaTime)
{
    std::cout << "Key pressed: " << key << std::endl;
    if(key == SDLK_Z) // Forward
        position += front * speed * deltaTime;
    if(key == SDLK_S) // Backward
        position -= front * speed * deltaTime;
    if(key == SDLK_Q) // Left
        position -= right * speed * deltaTime;
    if(key == SDLK_D) // Right
        position += right * speed * deltaTime;
    if(key == SDLK_SPACE) // Up
        position += up * speed * deltaTime;
    if(key == SDLK_LSHIFT) // Down
        position -= up * speed * deltaTime;
}
