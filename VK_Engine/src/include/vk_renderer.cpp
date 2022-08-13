#include "vk_renderer.hpp"

//std
#include <stdexcept>
#include <array>

namespace rvk {

    VK_Renderer::VK_Renderer(VK_window &window, VK_Device &device) : vk_window{window}, vk_device{device} {
        recreate_swapchain();
        create_command_buffers();
    }

    VK_Renderer::~VK_Renderer() {
        free_command_buffers();
    }

    void VK_Renderer::recreate_swapchain() {
        auto extend = vk_window.get_extend();

        while(extend.width == 0 || extend.height == 0) {
            extend = vk_window.get_extend();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(vk_device.device());
        //vk_swapchain = nullptr;

        if(vk_swapchain == nullptr){
            vk_swapchain = std::make_unique<VK_swapchain>(vk_device, extend);
        }else{
            std::shared_ptr<VK_swapchain> oldSwapchain = std::move(vk_swapchain);
            vk_swapchain = std::make_unique<VK_swapchain>(vk_device, extend, oldSwapchain);

            if (!oldSwapchain->compareSwapFormats(*vk_swapchain.get())) {
                throw std::runtime_error("image format has changed");
            }
        }

        //vk_pipeline = nullptr;
        //pipeline_layout = nullptr;
        //create_pipeline();
    }

    void VK_Renderer::create_command_buffers() {
        command_buffers.resize(VK_swapchain::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO; 
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandPool = vk_device.getCommandPool();

        alloc_info.commandBufferCount = static_cast<uint32_t>(command_buffers.size());

        if (vkAllocateCommandBuffers(vk_device.device(), &alloc_info, command_buffers.data()) != VK_SUCCESS){
            throw std::runtime_error("Failed to allocate command buffers");
        }

    }

    void VK_Renderer::free_command_buffers() {
        vkFreeCommandBuffers(vk_device.device(), vk_device.getCommandPool(), static_cast<uint32_t>(command_buffers.size()), command_buffers.data());
        command_buffers.clear();
    }

    VkCommandBuffer VK_Renderer::beginFrame() {
        assert(!isFrameStarted && "Can't call beginframe when framedraw is started.");

        //uint32_t image_index;
        auto result = vk_swapchain->acquireNextImage(&currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreate_swapchain();
            return nullptr;
        }
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("faildes to accuaire swapchain image");
        }

        isFrameStarted = true;

        auto command_buffer = getCurrentCommandBuffer();

        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            
        if(vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin command buffer");
        }

        return command_buffer;
    }

    void VK_Renderer::endFrame() {
        assert(isFrameStarted && "Can't call endframe when framedraw isn't started.");
        auto command_buffer = getCurrentCommandBuffer();


        if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to record command buffer");
        }

        auto result = vk_swapchain->submitCommandBuffers(&command_buffer, &currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vk_window.was_window_resized()) {
            vk_window.reset_resize_flag();
            recreate_swapchain();
        }else if (result != VK_SUCCESS) {
            throw std::runtime_error("faildes to present swapchain image");
        }

        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex+1)%VK_swapchain::MAX_FRAMES_IN_FLIGHT;
    }

    void VK_Renderer::beginSwapChainRenderPass(VkCommandBuffer command_buffer) {
        assert(isFrameStarted && "Can't call beginSwapChainRenderPass when framedraw isn't started.");
        assert(command_buffer == getCurrentCommandBuffer() && "Cant work on command buffer from diffrent frame");
        //auto command_buffer = getCurrentCommandBuffer();


            VkRenderPassBeginInfo renderpass_info{};
            renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderpass_info.renderPass = vk_swapchain->getRenderPass();
            renderpass_info.framebuffer = vk_swapchain->getFrameBuffer(currentImageIndex);

            renderpass_info.renderArea.offset = {0, 0};
            renderpass_info.renderArea.extent = vk_swapchain->getSwapChainExtent();

            std::array<VkClearValue, 2> clear_values{};
            clear_values[0].color = {0.01f, 0.01f, 0.01f, 1.0f};           //clear
            clear_values[1].depthStencil = {1.0f, 0};

            renderpass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
            renderpass_info.pClearValues = clear_values.data();

            vkCmdBeginRenderPass(command_buffer, &renderpass_info, VK_SUBPASS_CONTENTS_INLINE);

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = static_cast<float>(vk_swapchain->getSwapChainExtent().width);
            viewport.height = static_cast<float>(vk_swapchain->getSwapChainExtent().height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            VkRect2D scissor{{0, 0}, vk_swapchain->getSwapChainExtent()};
            vkCmdSetViewport(command_buffer, 0, 1, &viewport);
            vkCmdSetScissor(command_buffer, 0, 1, &scissor);

    }

    void VK_Renderer::endSwapChainRenderPass(VkCommandBuffer command_buffer) {
        assert(isFrameStarted && "Can't call endSwapChainRenderPass when framedraw isn't started.");
        assert(command_buffer == getCurrentCommandBuffer() && "Cant work on command buffer from diffrent frame");

        vkCmdEndRenderPass(command_buffer);
    }
}