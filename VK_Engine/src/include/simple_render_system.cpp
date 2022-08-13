#include "simple_render_system.hpp"

//render stuff
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//std
#include <stdexcept>
#include <array>
#include <iostream>

namespace rvk {

    struct simple_push_constant_data {
        glm::mat4 modelMatrix{1.f};
        glm::mat4 normalMatrix{1.f};
    };

    SimpleRenderSystem::SimpleRenderSystem(VK_Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : vk_device{device} {
        create_pipeline_layout(globalSetLayout);
        create_pipeline(renderPass);
    }

    SimpleRenderSystem::~SimpleRenderSystem() {
        vkDestroyPipelineLayout(vk_device.device(), pipeline_layout, nullptr);
    }

    void SimpleRenderSystem::create_pipeline_layout(VkDescriptorSetLayout globalSetLayout) {

        VkPushConstantRange push_const_range;
        push_const_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        push_const_range.offset = 0;
        push_const_range.size = sizeof(simple_push_constant_data);

        std::vector<VkDescriptorSetLayout> descriptorSetLayout{globalSetLayout};

        VkPipelineLayoutCreateInfo pipeline_layout_info{};
        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(descriptorSetLayout.size());
        pipeline_layout_info.pSetLayouts = descriptorSetLayout.data();
        pipeline_layout_info.pushConstantRangeCount = 1;
        pipeline_layout_info.pPushConstantRanges = &push_const_range;
        if (vkCreatePipelineLayout(vk_device.device(), &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS){
            throw std::runtime_error("Failed to create pipeline layout!");
        }
    }

    void SimpleRenderSystem::create_pipeline(VkRenderPass renderPass) {
        //pipeline_layout = nullptr;
        if(pipeline_layout == nullptr){
            std::cout << "What the actual FUCK!";
        }
        assert(pipeline_layout != nullptr && "Cannot create pipeline before pipeline layout");

        //auto pipeline_config = VK_pipeline::default_pipeline_config_info(vk_swapchain->width(), vk_swapchain->height());
        pipeline_config_info pipeline_config{};
        VK_pipeline::default_pipeline_config_info(pipeline_config);
        pipeline_config.render_pass = renderPass;
        pipeline_config.pipeline_layout = pipeline_layout;
        vk_pipeline = std::make_unique<VK_pipeline>(
            vk_device,
            "shaders/lit_shader.vert.spv",
            "shaders/lit_shader.frag.spv",
            pipeline_config);
    }

    void SimpleRenderSystem::render_gameobjects(FrameInfo &frameinfo) {
        vk_pipeline -> bind(frameinfo.command_buffer);

        //auto projectionView = frameinfo.camera.getProjection() * frameinfo.camera.getViewMatrix();
        vkCmdBindDescriptorSets(
            frameinfo.command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipeline_layout,
            0, 1,
            &frameinfo.globalDescriptorSet,
            0, nullptr);

        for (auto& kv: frameinfo.gameObjects) {
            auto& obj = kv.second;
            if (obj.model == nullptr) continue;
            simple_push_constant_data push{};
            //push.color = obj.color;
            push.modelMatrix = obj.transform.mat4();
            push.normalMatrix = obj.transform.normalMAtrix();

            vkCmdPushConstants(frameinfo.command_buffer, pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(simple_push_constant_data), &push );
            obj.model->bind(frameinfo.command_buffer);
            obj.model->draw(frameinfo.command_buffer);
        }
    }
}