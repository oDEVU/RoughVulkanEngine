#pragma once

#include "vk_device.hpp"
#include "vk_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

//std
#include <memory>
#include <vector>

namespace rvk {
    class VK_model {
        public:

            struct Vertex {
                glm::vec3 position{};
                glm::vec3 color{};
                glm::vec3 normal{};
                glm::vec2 tex_uv{};

                static std::vector<VkVertexInputBindingDescription> get_binding_descriptions();                
                static std::vector<VkVertexInputAttributeDescription> get_attribute_descriptions();

                bool operator==(const Vertex &other) const {
                    return position == other.position && color == other.color && normal == other.normal && tex_uv == other.tex_uv;
                }
            };

            struct Builder {
                std::vector<Vertex> vertices{};
                std::vector<uint32_t> indices{};

                void loadModel(const std::string &filepath);
            };

            VK_model(VK_Device &device, const VK_model::Builder &builder);
            ~VK_model();

            VK_model(const VK_model &) = delete;
            VK_model &operator=(const VK_model &) = delete; 

            static std::unique_ptr<VK_model> createModerFromFile(VK_Device &device, const std::string &filepath);

            void bind(VkCommandBuffer command_buffer);
            void draw(VkCommandBuffer command_buffer);

        private:
            void create_vertex_buffers(const std::vector<Vertex> &vertieces);
            void create_index_buffers(const std::vector<uint32_t> &indices);

            VK_Device& vk_device;

            std::unique_ptr<VK_Buffer> vertex_buffer;
            uint32_t vertex_count;

            bool hasIndexBuffer = false;
            std::unique_ptr<VK_Buffer> index_buffer;
            uint32_t index_count;
    };
}