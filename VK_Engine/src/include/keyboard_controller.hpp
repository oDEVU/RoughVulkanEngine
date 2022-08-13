#pragma once

#include "vk_game_object.hpp"
#include "vk_window.hpp"

namespace rvk {
    class KeyboardConstroller {
        public:
            struct KeyMappings {
                int moveLeft = GLFW_KEY_A;
                int moveRight = GLFW_KEY_D;
                int moveForward = GLFW_KEY_W;
                int moveBackward = GLFW_KEY_S;
                int moveUp = GLFW_KEY_LEFT_SHIFT;
                int moveDown = GLFW_KEY_LEFT_CONTROL;
                int lookLeft = GLFW_KEY_LEFT;
                int lookRight = GLFW_KEY_RIGHT;
                int lookUp = GLFW_KEY_UP;
                int lookDown = GLFW_KEY_DOWN;
            };

            void moveInPlaneXZ(GLFWwindow* window, float delta, VK_GameObject &gameobject);

            KeyMappings keys{};
            float movespeed{2.f};
            float turnspeed{1.f};
    };
}