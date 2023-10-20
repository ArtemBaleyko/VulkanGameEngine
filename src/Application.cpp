#include "Application.h"

#include <array>
#include <stdexcept>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace vge {

struct PushConstantData {
    glm::vec2 offset;
    alignas(16) glm::vec3 color;
};

Application::Application() {
    loadModels();
    createPipelineLayout();
    recreateSwapChain();
    createCommandBuffers();
}

Application::~Application() { vkDestroyPipelineLayout(_device.getVkDevice(), _pipelineLayout, nullptr); }

void Application::run() {
    while (!_window.shouldClose()) {
        glfwPollEvents();
        drawFrame();
    }

    vkDeviceWaitIdle(_device.getVkDevice());
}

void Application::loadModels() {
    std::vector<Model::Vertex> vertices = {
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f},  {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    };
    _model = std::make_unique<Model>(_device, vertices);
}

void Application::createPipelineLayout() {
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
void Application::createPipeline() {
    PipelineConfig pipelineConfig{};
    Pipeline::getDefaultPipelineConfigInfo(pipelineConfig);

    pipelineConfig.renderPass = _swapChain->getRenderPass();
    pipelineConfig.pipelineLayout = _pipelineLayout;

    _pipeline = std::make_unique<Pipeline>(
        "shaders/shader.vert.spv", "shaders/shader.frag.spv", _device, pipelineConfig);
}
void Application::createCommandBuffers() {
    _commandBuffers.resize(_swapChain->getImageCount());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = _device.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(_commandBuffers.size());

    if (vkAllocateCommandBuffers(_device.getVkDevice(), &allocInfo, _commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command buffers");
    }
}

void Application::freeCommandBuffers() {
    vkFreeCommandBuffers(_device.getVkDevice(),
                         _device.getCommandPool(),
                         static_cast<uint32_t>(_commandBuffers.size()),
                         _commandBuffers.data());

    _commandBuffers.clear();
}

void Application::drawFrame() {
    uint32_t imageIndex;
    auto result = _swapChain->acquireNextImage(&imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image");
    }

    recordCommandBuffer(imageIndex);
    result = _swapChain->submitCommandBuffers(&_commandBuffers[imageIndex], &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _window.wasResized()) {
        _window.resetWindowResizedFlag();
        recreateSwapChain();
        return;
    }

    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present image");
    }
}
void Application::recreateSwapChain() {
    auto extent = _window.getExtent();

    while (extent.width == 0 || extent.height == 0) {
        extent = _window.getExtent();
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(_device.getVkDevice());

    if (_swapChain == nullptr) {
        _swapChain = std::make_unique<SwapChain>(_device, extent);
    } else {
        _swapChain = std::make_unique<SwapChain>(_device, extent, std::move(_swapChain));
        if (_swapChain->getImageCount() != _commandBuffers.size()) {
            freeCommandBuffers();
            createCommandBuffers();
        }
    }

    createPipeline();
}

void Application::recordCommandBuffer(int imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(_commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording to command buffer");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = _swapChain->getRenderPass();
    renderPassInfo.framebuffer = _swapChain->getFrameBuffer(imageIndex);
    renderPassInfo.renderArea.extent = _swapChain->getSwapChainExtent();
    renderPassInfo.renderArea.offset = {0, 0};

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
    clearValues[1].depthStencil = {0.1f, 0};

    renderPassInfo.clearValueCount = 2;
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(_commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f, viewport.y = 0.0f;
    viewport.width = static_cast<float>(_swapChain->getSwapChainExtent().width);
    viewport.height = static_cast<float>(_swapChain->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{
        {0, 0},
        _swapChain->getSwapChainExtent()
    };

    vkCmdSetViewport(_commandBuffers[imageIndex], 0, 1, &viewport);
    vkCmdSetScissor(_commandBuffers[imageIndex], 0, 1, &scissor);

    _pipeline->bind(_commandBuffers[imageIndex]);
    _model->bind(_commandBuffers[imageIndex]);

    for (int i = 0; i < 4; i++) {
        PushConstantData data;
        data.offset = {0.0f, -0.4f + i * 0.25f};
        data.color = {0.0f, 0.0f, 0.2f + 0.2f * i};

        vkCmdPushConstants(_commandBuffers[imageIndex],
                           _pipelineLayout,
                           VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                           0,
                           sizeof(PushConstantData),
                           &data);

        _model->draw(_commandBuffers[imageIndex]);
    }

    vkCmdEndRenderPass(_commandBuffers[imageIndex]);

    if (vkEndCommandBuffer(_commandBuffers[imageIndex]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer");
    }
}
}  // namespace vge