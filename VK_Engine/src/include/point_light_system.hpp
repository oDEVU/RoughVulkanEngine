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
    
    class PointLightSystem {
        public:
            PointLightSystem(VK_Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
            ~PointLightSystem();

            PointLightSystem(const PointLightSystem &) = delete;
            PointLightSystem& operator=(const PointLightSystem &) = delete;  

            void update(FrameInfo &frameinfo, GlobaUbo &ubo);
            void render(FrameInfo &frameinfo);
        
        private:
            void create_pipeline_layout(VkDescriptorSetLayout globalSetLayout);
            void create_pipeline(VkRenderPass renderPass);

            VK_Device& vk_device;
            
            std::unique_ptr<VK_pipeline> vk_pipeline;
            VkPipelineLayout pipeline_layout;
    };
}