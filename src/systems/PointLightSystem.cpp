#include "PointLightSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <array>
#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <stdexcept>

namespace vge {

PointLightSystem::PointLightSystem(Device& device,
                                   VkRenderPass renderPass,
                                   VkDescriptorSetLayout globalSetLayout)
    : _device{device} {
    createPipelineLayout(globalSetLayout);
    createPipeline(renderPass);
}

PointLightSystem::~PointLightSystem() {
    vkDestroyPipelineLayout(_device.getVkDevice(), _pipelineLayout, nullptr);
}

void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
    // VkPushConstantRange pushConstantRange{};
    // pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    // pushConstantRange.offset = 0;
    // pushConstantRange.size = sizeof(PushConstantData);

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
    if (vkCreatePipelineLayout(_device.getVkDevice(), &pipelineLayoutInfo, nullptr, &_pipelineLayout) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void PointLightSystem::createPipeline(VkRenderPass renderPass) {
    assert(_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    Pipeline::defaultPipelineConfigInfo(pipelineConfig);

    pipelineConfig.attributeDescriptions.clear();
    pipelineConfig.bindingDescriptions.clear();

    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = _pipelineLayout;
    _pipeline = std::make_unique<Pipeline>(
        _device, "../shaders/point_light.vert.spv", "../shaders/point_light.frag.spv", pipelineConfig);
}

void PointLightSystem::render(FrameInfo& frameInfo) {
    auto commandBuffer = frameInfo.commandBuffer;
    _pipeline->bind(commandBuffer);

    vkCmdBindDescriptorSets(commandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            _pipelineLayout,
                            0,
                            1,
                            &frameInfo.globalDescriptorSet,
                            0,
                            nullptr);

    vkCmdDraw(commandBuffer, 6, 1, 0, 0);
}
}  // namespace vge
