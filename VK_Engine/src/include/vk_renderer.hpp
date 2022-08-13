#pragma once

#include "vk_window.hpp"
#include "vk_device.hpp"
#include "vk_swapchain.hpp"

//std
#include <memory>
#include <vector>
#include <cassert>

namespace rvk {
    class VK_Renderer {
        public:
            VK_Renderer(VK_window &window, VK_Device &device);
            ~VK_Renderer();

            VK_Renderer(const VK_Renderer &) = delete;
            VK_Renderer& operator=(const VK_Renderer &) = delete; 
            
            VkRenderPass getSwapChainRenderPass() const { return vk_swapchain->getRenderPass(); }

            float getAspectRatio() const { return vk_swapchain->extentAspectRatio(); }

            bool isFrameInProgress() const { return isFrameStarted; }
            VkCommandBuffer getCurrentCommandBuffer() const { 
                assert(isFrameStarted && "Cannot get command buffer when frame is not in progress");
                return command_buffers[currentFrameIndex]; 
                }

            int getFrameIndex() const {
                assert(isFrameStarted && "Cannot get frame index when frame is not in progress");
                return currentFrameIndex;
            }

            VkCommandBuffer beginFrame();
            void endFrame();
            void beginSwapChainRenderPass(VkCommandBuffer command_buffer);
            void endSwapChainRenderPass(VkCommandBuffer command_buffer);
        
        private:
            void create_command_buffers();
            void free_command_buffers();
            void recreate_swapchain();

            VK_window& vk_window;
            VK_Device& vk_device;
            //VK_swapchain vk_swapchain{vk_device, vk_window.get_extend()};
            //VK_pipeline vk_pipeline{vk_device, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", VK_pipeline::default_pipeline_config_info(WIDTH, HEIGHT)};
            std::unique_ptr<VK_swapchain> vk_swapchain;
            std::vector<VkCommandBuffer> command_buffers;

            uint32_t currentImageIndex;
            int currentFrameIndex = 0;
            bool isFrameStarted = false;
    };
}