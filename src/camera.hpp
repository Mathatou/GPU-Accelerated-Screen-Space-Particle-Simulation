#pragma once
#include <glm/glm.hpp>
#include <SDL3/SDL.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

class Camera 
{
    public:
        Camera();
        void Update();
        glm::mat4 GetViewMatrix(glm::mat4& view);
        void OnMouseMove(float xoffset, float yoffset);
        void OnKeyboardInput(int key, float deltaTime);
        void OnMouseScroll(float yoffset);

    private:
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        glm::vec3 right;
        glm::vec3 worldUp;
        float yaw;
        float pitch;
        float speed;
        float sensitivity;
};