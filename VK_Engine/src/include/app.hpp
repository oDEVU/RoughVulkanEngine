#pragma once

#include "vk_window.hpp"
#include "vk_device.hpp"
#include "vk_game_object.hpp"
#include "vk_renderer.hpp"
#include "vk_descriptors.hpp"

//render stuff
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//std
#include <memory>
#include <vector>

namespace rvk {
    
    class App {
        public:
            static constexpr int WIDTH = 1280;
            static constexpr int HEIGHT = 720;

            App();
            ~App();

            App(const App &) = delete;
            App& operator=(const App &) = delete;  
            
            void run();
        
        private:
            void load_gameobjects();

            VK_window vk_window{WIDTH, HEIGHT, "Rough Vulkan Engine [ver. i wanna die]"};
            VK_Device vk_device{vk_window};
            VK_Renderer vk_renderer{vk_window, vk_device};
            
            std::unique_ptr<VK_DescriptorPool> globalPool{};
            VK_GameObject::Map vk_gameobjects;
    };
}