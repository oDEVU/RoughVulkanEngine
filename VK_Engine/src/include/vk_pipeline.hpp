#pragma once

#include "vk_device.hpp"

//std
#include <string>
#include <vector>

namespace rvk {
    struct pipeline_config_info {
        pipeline_config_info(const pipeline_config_info&) = delete;
        pipeline_config_info& operator=(const pipeline_config_info&) = delete;

        std::vector<VkVertexInputAttributeDescription> attributeDesc{};
        std::vector<VkVertexInputBindingDescription> bindingDesc{};
        VkPipelineViewportStateCreateInfo viewport_info;
        VkPipelineInputAssemblyStateCreateInfo input_assembly_info;
        VkPipelineRasterizationStateCreateInfo rasterization_info;
        VkPipelineMultisampleStateCreateInfo multisample_info;
        VkPipelineColorBlendAttachmentState color_blend_attachment;
        VkPipelineColorBlendStateCreateInfo color_blend_info;
        VkPipelineDepthStencilStateCreateInfo depth_stencil_info;
        std::vector<VkDynamicState> dynamic_state_enables;
        VkPipelineDynamicStateCreateInfo dynamic_state_info;
        VkPipelineLayout pipeline_layout = nullptr;
        VkRenderPass render_pass = nullptr;
        uint32_t subpass = 0;
    };

    class VK_pipeline {
        public:
            VK_pipeline(
                VK_Device &device, 
                const std::string& vert_filepath, 
                const std::string& frag_filepath, 
                const pipeline_config_info& config_info);
            
            ~VK_pipeline();

            VK_pipeline(const VK_pipeline&) = delete;
            VK_pipeline& operator=(const VK_pipeline&) = delete;

            void bind(VkCommandBuffer command_buffer);

            static void default_pipeline_config_info(pipeline_config_info& config_info);
        
        private:
            static std::vector<char> read_file(const std::string& filepath);

            void create_gfx_pipeline(
                const std::string& vert_filepath,
                const std::string& frag_filepath,
                const pipeline_config_info& config_info);
            
            void create_shader_module(
                const std::vector<char>& code,
                VkShaderModule* shader_module);
            
            VK_Device& vk_device;
            VkPipeline gfx_pipeline;
            VkShaderModule vert_shader_module;
            VkShaderModule frag_shader_module;
    };
}