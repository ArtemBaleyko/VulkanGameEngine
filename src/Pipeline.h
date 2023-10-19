#pragma once

#include <string>

#include "Device.h"

namespace vge {

struct PipelineConfig {
    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;
};

class Pipeline {
public:
    Pipeline(const std::string& vertShaderPath, const std::string& fragShaderPath, Device& device,
             const PipelineConfig& config);

    ~Pipeline();

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    void bind(VkCommandBuffer commandBuffer);

    static PipelineConfig getDefaultPipelineConfigInfo(uint32_t width, uint32_t height);

private:
    void createGraphicsPipeline(const std::string& vertShaderPath, const std::string& fragShaderPath,
                                const PipelineConfig& config);

    void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

private:
    Device& _device;
    VkPipeline _graphicsPipeline;
    VkShaderModule _vertShaderModule;
    VkShaderModule _fragShaderModule;
};
}  // namespace vge
