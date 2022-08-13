#pragma once

#include "vk_camera.hpp"
#include "vk_game_object.hpp"

#include <vulkan/vulkan.h>

namespace rvk {
    struct FrameInfo {
        int frameindex;
        float frametime;
        VkCommandBuffer command_buffer;
        VK_Camera &camera;
        VkDescriptorSet globalDescriptorSet;
        VK_GameObject::Map &gameObjects;
    };

    #define MAX_LIGHTS 256

    struct PointLight {
        glm::vec4 position{};
        glm::vec4 color{};
    };

    struct GlobaUbo {
        alignas(16) glm::mat4 projection{1.f};
        alignas(16) glm::mat4 view{1.f};
        alignas(16) glm::mat4 inverseView{1.f};
        alignas(16) glm::vec4 ambienLight{0.8f, 0.8f, 1.0f, 0.03f};
        PointLight pointLights[MAX_LIGHTS];
        int numLights;
    };
} // namespace rvk
