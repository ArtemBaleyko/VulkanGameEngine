#include "RenderSystem.h"

#include <array>
#include <stdexcept>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace vge {

struct PushConstantData {
    glm::mat2 transform{1.0f};
    glm::vec2 offset;
    alignas(16) glm::vec3 color;
};

RenderSystem::RenderSystem(Device& device, VkRenderPass renderPass)
    : _device{device} {
    createPipelineLayout();
    createPipeline(renderPass);
}

RenderSystem::~RenderSystem() { vkDestroyPipelineLayout(_device.getVkDevice(), _pipelineLayout, nullptr); }

void RenderSystem::createPipelineLayout() {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(PushConstantData);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(_device.getVkDevice(), &pipelineLayoutInfo, nullptr, &_pipelineLayout) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout");
    }
}
void RenderSystem::createPipeline(VkRenderPass renderPass) {
    PipelineConfig pipelineConfig{};
    Pipeline::getDefaultPipelineConfigInfo(pipelineConfig);

    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = _pipelineLayout;

    _pipeline = std::make_unique<Pipeline>(
        "shaders/shader.vert.spv", "shaders/shader.frag.spv", _device, pipelineConfig);
}

void RenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, const std::vector<GameObject>& gameObjects) {
    _pipeline->bind(commandBuffer);

    for (const auto& obj : gameObjects) {
        PushConstantData data;
        data.offset = obj.transform2D.translation;
        data.color = obj.color;
        data.transform = obj.transform2D.mat2();

        vkCmdPushConstants(commandBuffer,
                           _pipelineLayout,
                           VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                           0,
                           sizeof(PushConstantData),
                           &data);

        obj.model->bind(commandBuffer);
        obj.model->draw(commandBuffer);
    }
}
}  // namespace vge