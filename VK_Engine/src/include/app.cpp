#include "app.hpp"

#include "simple_render_system.hpp"
#include "vk_camera.hpp"
#include "keyboard_controller.hpp"
#include "vk_buffer.hpp"
#include "point_light_system.hpp"

//std
#include <stdexcept>
#include <array>
#include <iostream>
#include <chrono>

namespace rvk {
    App::App() {
        globalPool = VK_DescriptorPool::Builder(vk_device)
            .setMaxSets(VK_swapchain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_swapchain::MAX_FRAMES_IN_FLIGHT)
            .build();
        load_gameobjects();
    }

    App::~App() {

    }

    void App::run() {
        std::vector<std::unique_ptr<VK_Buffer>> uboBuffers(VK_swapchain::MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < uboBuffers.size(); i++){
            uboBuffers[i] = std::make_unique<VK_Buffer>(
            vk_device,
            sizeof(GlobaUbo),
            1,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

            uboBuffers[i]->map();
        }

        auto globalSetLayout = VK_DescriptorSetLayout::Builder(vk_device)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .build();
        
        std::vector<VkDescriptorSet> globalDescriptorSets(VK_swapchain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++){
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            VK_DescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }

        SimpleRenderSystem simpleRenderSystem{vk_device, vk_renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
        PointLightSystem pointLightSystem{vk_device, vk_renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
        VK_Camera camera{};
        //camera.setViewDirection(glm::vec3{0.f}, glm::vec3{.5f, .0f, 1.f});

        auto viewerObject = VK_GameObject::create_game_object();
        KeyboardConstroller cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        while(!vk_window.should_close()) {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ(vk_window.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = vk_renderer.getAspectRatio();
            //camera.setOrhographicProjection(-aspect, aspect, -1, 1, -1 ,1);
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.0f);

            if (auto command_buffer = vk_renderer.beginFrame()){
                int frameindex = vk_renderer.getFrameIndex();
                FrameInfo frameinfo{
                    frameindex,
                    frameTime,
                    command_buffer,
                    camera,
                    globalDescriptorSets[frameindex],
                    vk_gameobjects,
                };

                //update
                GlobaUbo ubo{};
                ubo.projection = camera.getProjection();
                ubo.view = camera.getViewMatrix();
                ubo.inverseView = camera.getInverseViewMatrix();
                pointLightSystem.update(frameinfo, ubo);
                uboBuffers[frameindex]->writeToBuffer(&ubo);
                uboBuffers[frameindex]->flush();
                
                //render
                vk_renderer.beginSwapChainRenderPass(command_buffer);
                simpleRenderSystem.render_gameobjects(frameinfo);
                pointLightSystem.render(frameinfo);
                vk_renderer.endSwapChainRenderPass(command_buffer);
                vk_renderer.endFrame();
            }
        }

        vkDeviceWaitIdle(vk_device.device());
    }

    void App::load_gameobjects() {
        std::shared_ptr<VK_model> Model = VK_model::createModerFromFile(vk_device, "basic/models/cube.obj");

        auto cube = VK_GameObject::create_game_object();

        cube.model = Model;
        cube.transform.translation = {-1.0f, .0f, 0.f};
        cube.transform.scale = {.5f, .5f, .5f};

        vk_gameobjects.emplace(cube.get_id(), std::move(cube));

        std::shared_ptr<VK_model> Model2 = VK_model::createModerFromFile(vk_device, "basic/models/colored_cube.obj");

        auto cube_col = VK_GameObject::create_game_object();

        cube_col.model = Model2;
        cube_col.transform.translation = {1.f, .0f, 0.f};
        cube_col.transform.scale = {.5f, .5f, .5f};

        vk_gameobjects.emplace(cube_col.get_id(), std::move(cube_col));

        std::shared_ptr<VK_model> Model3 = VK_model::createModerFromFile(vk_device, "basic/models/teapot.obj");

        auto teapot = VK_GameObject::create_game_object();

        teapot.model = Model3;
        teapot.transform.translation = {1.f, -0.5f, 0.f};
        teapot.transform.scale = {.3f, .3f, .3f};

        vk_gameobjects.emplace(teapot.get_id(), std::move(teapot));

        std::shared_ptr<VK_model> Model4 = VK_model::createModerFromFile(vk_device, "basic/models/smooth_vase.obj");

        auto vase = VK_GameObject::create_game_object();

        vase.model = Model4;
        vase.transform.translation = {-1.3f, -0.5f, .2f};
        vase.transform.scale = {2.f, 2.f, 2.f};

        vk_gameobjects.emplace(vase.get_id(), std::move(vase));

        std::shared_ptr<VK_model> Model5 = VK_model::createModerFromFile(vk_device, "basic/models/flat_vase.obj");

        auto vase2 = VK_GameObject::create_game_object();

        vase2.model = Model5;
        vase2.transform.translation = {-0.7f, -0.5f, -0.2f};
        vase2.transform.scale = {2.f, 2.f, 2.f};

        vk_gameobjects.emplace(vase2.get_id(), std::move(vase2));

        std::shared_ptr<VK_model> Model6 = VK_model::createModerFromFile(vk_device, "basic/models/quad.obj");

        auto qd = VK_GameObject::create_game_object();

        qd.model = Model6;
        qd.transform.translation = {0.f, 0.5f, 0.f};
        qd.transform.scale = {5.f, 5.f, 5.f};

        vk_gameobjects.emplace(qd.get_id(), std::move(qd));

        /*
        auto pointLight = VK_GameObject::makePointLight(1.f);
        pointLight.transform.translation = {0.f, -2.5f, 0.f};
        vk_gameobjects.emplace(pointLight.get_id(), std::move(pointLight));
        */

        std::vector<glm::vec3> lightColors{
            {1.f, .1f, .1f},
             {.1f, .1f, 1.f},
            {.1f, 1.f, .1f},
            {1.f, 1.f, .1f},
            {.1f, 1.f, 1.f},
            {1.f, 1.f, 1.f}  //
        };

        for (int i = 0; i < lightColors.size(); i++){
            auto pointLight = VK_GameObject::makePointLight(1.f);
            pointLight.color = lightColors[i];

            auto rotateLights = glm::rotate(glm::mat4(1.f), (i*glm::two_pi<float>()) / lightColors.size(), {0.f, -1.f, 0.f});   
            pointLight.transform.translation = glm::vec3(rotateLights * glm::vec4{-1.f, -2.f, -2.5f, 2.f});

            vk_gameobjects.emplace(pointLight.get_id(), std::move(pointLight));
        }

    }

}