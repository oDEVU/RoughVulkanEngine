#include "keyboard_controller.hpp"

#include <limits>

namespace rvk {
    void KeyboardConstroller::moveInPlaneXZ(GLFWwindow* window, float delta, VK_GameObject &gameobject) {
        glm::vec3 rotate{.0f};

        if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) {
            rotate.y += 1.f;
        }
        if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) {
            rotate.y -= 1.f;
        }
        if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) {
            rotate.x += 1.f;
        }
        if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) {
            rotate.x -= 1.f;
        }

        if(glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
            gameobject.transform.rotation += turnspeed * delta * glm::normalize(rotate);
        }

        gameobject.transform.rotation.x = glm::clamp(gameobject.transform.rotation.x, -1.5f, 1.5f);
        gameobject.transform.rotation.y = glm::mod(gameobject.transform.rotation.y, glm::two_pi<float>());

        float yaw = gameobject.transform.rotation.y;
        const glm::vec3 forwardDir{sin(yaw), 0.f, cos(yaw)};
        const glm::vec3 rightDir{forwardDir.z, .0f, -forwardDir.x};
        const glm::vec3 upDir{0.f, -1.f, 0.f};

        glm::vec3 moveDir{0.f};
        if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) {
            moveDir += forwardDir;
        }
        if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) {
            moveDir -= forwardDir;
        }
        if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) {
            moveDir += rightDir;
        }
        if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) {
            moveDir -= rightDir;
        }
        if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) {
            moveDir += upDir;
        }
        if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) {
            moveDir -= upDir;
        }

        if(glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
            gameobject.transform.translation += movespeed * delta * glm::normalize(moveDir);
        }
    }
}