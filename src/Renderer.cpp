#include "Renderer.h"

#include <array>
#include <stdexcept>

namespace vge {

Renderer::Renderer(Window& window, Device& device)
    : _window{window}
    , _device{device}
    , _currentImageIndex{0}
    , _currentFrameIndex{0}
    , _isFrameStarted{false} {
    recreateSwapChain();
    createCommandBuffers();
}

Renderer::~Renderer() { freeCommandBuffers(); }

VkCommandBuffer Renderer::beginFrame() {
    assert(!_isFrameStarted && "Frame is already in progress");

    auto result = _swapChain->acquireNextImage(&_currentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return nullptr;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image");
    }

    _isFrameStarted = true;

    auto currentCommandBuffer = getCurrentCommandBuffer();

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(currentCommandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording to command buffer");
    }

    return currentCommandBuffer;
}

void Renderer::endFrame() {
    assert(_isFrameStarted && "Frame has to be started first");

    auto commandBuffer = getCurrentCommandBuffer();

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer");
    }

    auto result = _swapChain->submitCommandBuffers(&commandBuffer, &_currentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _window.wasResized()) {
        _window.resetWindowResizedFlag();
        recreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present image");
    }

    _isFrameStarted = false;
    _currentFrameIndex = (_currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
}

void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
    assert(_isFrameStarted && "Can`t call beginSwapChainRenderPass is frame is already in progress");
    assert(commandBuffer == getCurrentCommandBuffer() &&
           "Can`t begin render pass on command buffer from different frame");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = _swapChain->getRenderPass();
    renderPassInfo.framebuffer = _swapChain->getFrameBuffer(_currentImageIndex);
    renderPassInfo.renderArea.extent = _swapChain->getSwapChainExtent();
    renderPassInfo.renderArea.offset = {0, 0};

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
    clearValues[1].depthStencil = {0.1f, 0};

    renderPassInfo.clearValueCount = 2;
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(_swapChain->getSwapChainExtent().width);
    viewport.height = static_cast<float>(_swapChain->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{
        {0, 0},
        _swapChain->getSwapChainExtent()
    };

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
    assert(_isFrameStarted && "Can`t call endSwapChainRenderPass is frame is already in progress");
    assert(commandBuffer == getCurrentCommandBuffer() &&
           "Can`t end render pass on command buffer from different frame");

    vkCmdEndRenderPass(commandBuffer);
}

void Renderer::createCommandBuffers() {
    _commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = _device.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(_commandBuffers.size());

    if (vkAllocateCommandBuffers(_device.getVkDevice(), &allocInfo, _commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command buffers");
    }
}

void Renderer::freeCommandBuffers() {
    vkFreeCommandBuffers(_device.getVkDevice(),
                         _device.getCommandPool(),
                         static_cast<uint32_t>(_commandBuffers.size()),
                         _commandBuffers.data());

    _commandBuffers.clear();
}

void Renderer::recreateSwapChain() {
    auto extent = _window.getExtent();

    while (extent.width == 0 || extent.height == 0) {
        extent = _window.getExtent();
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(_device.getVkDevice());

    if (_swapChain == nullptr) {
        _swapChain = std::make_unique<SwapChain>(_device, extent);
    } else {
        std::shared_ptr<SwapChain> oldSwapChain = std::move(_swapChain);
        _swapChain = std::make_unique<SwapChain>(_device, extent, oldSwapChain);

        if (!oldSwapChain->compareSwapFormats(*_swapChain.get())) {
            throw std::runtime_error("Swap chain image/depth format has changed!");
        }
    }
}
}  // namespace vge