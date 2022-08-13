#include "vk_model.hpp"

#include "vk_utils.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "external/tiny_obj_loader.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <cassert>
#include <cstring>
#include <iostream>
#include <unordered_map>

namespace std {
    template <>
    struct hash<rvk::VK_model::Vertex> {
        size_t operator()(rvk::VK_model::Vertex const &vertex) const {
            size_t seed = 0;
            rvk::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.tex_uv);
            return seed;
        }
    };
}

namespace rvk {
    VK_model::VK_model(VK_Device &device, const VK_model::Builder &builder) : vk_device{device} {
        create_vertex_buffers(builder.vertices);
        create_index_buffers(builder.indices);
    }

    VK_model::~VK_model(){

    }

    std::unique_ptr<VK_model> VK_model::createModerFromFile(VK_Device &device, const std::string &filepath) {
        Builder builder{};
        builder.loadModel(filepath);

        std::cout << "Loaded "<<builder.vertices.size()<<" vertices to memory.\n";

        return std::make_unique<VK_model>(device, builder);
    }

    void VK_model::create_vertex_buffers(const std::vector<Vertex> &vertieces) {
        vertex_count = static_cast<uint32_t>(vertieces.size());
        assert(vertex_count >= 3 && "Vertex count must be at least 3");

        VkDeviceSize buffer_size = sizeof(vertieces[0]) * vertex_count;
        uint32_t vertexSize = sizeof(vertieces[0]);

        VK_Buffer stagingBuffer{
            vk_device,
            vertexSize,
            vertex_count,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        };

        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void *)vertieces.data());

        vertex_buffer = std::make_unique<VK_Buffer>(
            vk_device,
            vertexSize,
            vertex_count,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


        vk_device.copyBuffer(stagingBuffer.getBuffer(), vertex_buffer->getBuffer(), buffer_size);
    }

    void VK_model::create_index_buffers(const std::vector<uint32_t> &indices) {
        index_count = static_cast<uint32_t>(indices.size());
        hasIndexBuffer = index_count > 0;

        if(!hasIndexBuffer){
            return;
        }

        VkDeviceSize buffer_size = sizeof(indices[0]) * index_count;
        uint32_t indexSize = sizeof(indices[0]);

        VK_Buffer stagingBuffer{
            vk_device,
            indexSize,
            index_count,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        };

        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void *)indices.data());

        index_buffer = std::make_unique<VK_Buffer>(
            vk_device,
            indexSize,
            index_count,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


        vk_device.copyBuffer(stagingBuffer.getBuffer(), index_buffer->getBuffer(), buffer_size);
    }

    void VK_model::draw(VkCommandBuffer command_buffer) {
        if(hasIndexBuffer){
            vkCmdDrawIndexed(command_buffer, index_count, 1, 0, 0, 0);
        }else{
            vkCmdDraw(command_buffer, vertex_count, 1, 0, 0);
        }
    }

    void VK_model::bind(VkCommandBuffer command_buffer) {
        VkBuffer buffers[] = {vertex_buffer->getBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers, offsets);

        if (hasIndexBuffer){
            vkCmdBindIndexBuffer(command_buffer, index_buffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
        }
    }

    std::vector<VkVertexInputBindingDescription> VK_model::Vertex::get_binding_descriptions() {
        std::vector<VkVertexInputBindingDescription> binding_descriptons{1};
        binding_descriptons[0].binding = 0;
        binding_descriptons[0].stride = sizeof(Vertex);
        binding_descriptons[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return binding_descriptons;
    } 

    std::vector<VkVertexInputAttributeDescription> VK_model::Vertex::get_attribute_descriptions() {
        std::vector<VkVertexInputAttributeDescription> attribute_descriptons{};

        attribute_descriptons.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
        attribute_descriptons.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
        attribute_descriptons.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
        attribute_descriptons.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, tex_uv)});

        return attribute_descriptons;
    }

    void VK_model::Builder::loadModel(const std::string &filepath) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if(!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())){
            throw std::runtime_error(warn + err);
        }

        vertices.clear();
        indices.clear();

        std::unordered_map<Vertex, uint32_t> uniqueVert{};

        for (const auto &shape : shapes) {
            for (const auto &index : shape.mesh.indices){
                Vertex vertex{};

                if (index.vertex_index >= 0) {
                    vertex.position = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };
                    vertex.color = {
                        attrib.colors[3 * index.vertex_index + 0],
                        attrib.colors[3 * index.vertex_index + 1],
                        attrib.colors[3 * index.vertex_index + 2]
                    };
                }
                if (index.normal_index >= 0) {
                    vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    };
                }
                if (index.texcoord_index >= 0) {
                    vertex.tex_uv = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        attrib.texcoords[2 * index.texcoord_index + 1]
                    };
                }

                //vertices.push_back(vertex);

                if (uniqueVert.count(vertex) == 0) {
                    uniqueVert[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }
                indices.push_back(uniqueVert[vertex]);
            } 
        }
    }
}