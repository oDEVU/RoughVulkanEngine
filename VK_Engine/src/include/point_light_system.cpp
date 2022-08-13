#include "point_light_system.hpp"

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
    struct PointLightPushConstants {
        glm::vec4 position{};
        glm::vec4 color{};
        float radius;
    };

    PointLightSystem::PointLightSystem(VK_Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : vk_device{device} {
        create_pipeline_layout(globalSetLayout);
        create_pipeline(renderPass);
    }

    PointLightSystem::~PointLightSystem() {
        vkDestroyPipelineLayout(vk_device.device(), pipeline_layout, nullptr);
    }

    void PointLightSystem::create_pipeline_layout(VkDescriptorSetLayout globalSetLayout) {

        VkPushConstantRange push_const_range;
        push_const_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        push_const_range.offset = 0;
        push_const_range.size = sizeof(PointLightPushConstants);

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

    void PointLightSystem::create_pipeline(VkRenderPass renderPass) {
        //pipeline_layout = nullptr;
        if(pipeline_layout == nullptr){
            std::cout << "What the actual FUCK!";
        }
        assert(pipeline_layout != nullptr && "Cannot create pipeline before pipeline layout");

        //auto pipeline_config = VK_pipeline::default_pipeline_config_info(vk_swapchain->width(), vk_swapchain->height());
        pipeline_config_info pipeline_config{};
        VK_pipeline::default_pipeline_config_info(pipeline_config);
        pipeline_config.attributeDesc.clear();
        pipeline_config.bindingDesc.clear();
        pipeline_config.render_pass = renderPass;
        pipeline_config.pipeline_layout = pipeline_layout;
        vk_pipeline = std::make_unique<VK_pipeline>(
            vk_device,
            "shaders/point_light.vert.spv",
            "shaders/point_light.frag.spv",
            pipeline_config);
    }

    void PointLightSystem::update(FrameInfo &frameinfo, GlobaUbo &ubo) {
        int lightIndex = 0;
        for (auto& kv: frameinfo.gameObjects){
            auto& obj = kv.second;
            if (obj.pointLight == nullptr){
                continue;
            }

            ubo.pointLights[lightIndex].position = glm::vec4(obj.transform.translation, 0.0f);
            ubo.pointLights[lightIndex].color = glm::vec4(obj.color, obj.pointLight->lightIntesity);
            lightIndex++;
        }

        ubo.numLights = lightIndex;
    }

    void PointLightSystem::render(FrameInfo &frameinfo) {
        vk_pipeline -> bind(frameinfo.command_buffer);

        //auto projectionView = frameinfo.camera.getProjection() * frameinfo.camera.getViewMatrix();
        vkCmdBindDescriptorSets(
            frameinfo.command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipeline_layout,
            0, 1,
            &frameinfo.globalDescriptorSet,
            0, nullptr);

        for (auto& kv: frameinfo.gameObjects){
            auto& obj = kv.second;
            if (obj.pointLight == nullptr){
                continue;
            }

                PointLightPushConstants push{};
                push.position = glm::vec4(obj.transform.translation, 0.0f);
                push.color = glm::vec4(obj.color, obj.pointLight->lightIntesity);
                push.radius = obj.transform.scale.x;

                vkCmdPushConstants(
                    frameinfo.command_buffer,
                    pipeline_layout,
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                    0,
                    sizeof(PointLightPushConstants),
                    &push);
                vkCmdDraw(frameinfo.command_buffer, 6, 1, 0, 0);
            
        }
    }
}