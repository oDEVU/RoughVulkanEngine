#pragma once

#include "vk_pipeline.hpp"
#include "vk_device.hpp"
#include "vk_game_object.hpp"
#include "vk_camera.hpp"
#include "vk_frameinfo.hpp"

//std
#include <memory>
#include <vector>

namespace rvk {
    
    class SimpleRenderSystem {
        public:
            SimpleRenderSystem(VK_Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
            ~SimpleRenderSystem();

            SimpleRenderSystem(const SimpleRenderSystem &) = delete;
            SimpleRenderSystem& operator=(const SimpleRenderSystem &) = delete;  

            void render_gameobjects(FrameInfo &frameinfo);
        
        private:
            void create_pipeline_layout(VkDescriptorSetLayout globalSetLayout);
            void create_pipeline(VkRenderPass renderPass);

            VK_Device& vk_device;
            
            std::unique_ptr<VK_pipeline> vk_pipeline;
            VkPipelineLayout pipeline_layout;
    };
}