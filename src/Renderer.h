#pragma once

#include <cassert>
#include <memory>
#include <vector>

#include "Device.h"
#include "Model.h"
#include "SwapChain.h"
#include "Window.h"

namespace vge {
class Renderer {
public:
    Renderer(Window& window, Device& device);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    inline VkRenderPass getSwapChainRenderPass() const { return _swapChain->getRenderPass(); }

    inline bool isFrameInProgress() const { return _isFrameStarted; }

    inline int getFrameIndex() const {
        assert(_isFrameStarted && "Cannot get frame index when frame is not started");
        return _currentFrameIndex;
    }

    inline VkCommandBuffer getCurrentCommandBuffer() const {
        assert(_isFrameStarted && "Cannot get command buffer when frame is not started");
        return _commandBuffers[_currentFrameIndex];
    }

    VkCommandBuffer beginFrame();
    void endFrame();

    void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

private:
    void createCommandBuffers();
    void freeCommandBuffers();
    void recreateSwapChain();

private:
    Window& _window;
    Device& _device;
    std::unique_ptr<SwapChain> _swapChain;
    std::vector<VkCommandBuffer> _commandBuffers;

    uint32_t _currentImageIndex;
    int _currentFrameIndex;
    bool _isFrameStarted;
};
}  // namespace vge