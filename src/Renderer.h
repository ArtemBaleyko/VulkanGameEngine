#pragma once

#include "Device.h"
#include "SwapChain.h"
#include "Window.h"

#include <cassert>
#include <memory>
#include <vector>

namespace vge {
class Renderer {
public:
    Renderer(Window& window, Device& device);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer &operator=(const Renderer &) = delete;

    inline VkRenderPass getSwapChainRenderPass() const { return _swapChain->getRenderPass(); }
    inline float getAspectRatio() const { return _swapChain->extentAspectRatio(); }
    inline bool isFrameInProgress() const { return _isFrameStarted; }

    inline VkCommandBuffer getCurrentCommandBuffer() const {
        assert(_isFrameStarted && "Cannot get command buffer when frame not in progress");
        return _commandBuffers[_currentFrameIndex];
    }

    inline int getFrameIndex() const {
        assert(_isFrameStarted && "Cannot get frame index when frame not in progress");
        return _currentFrameIndex;
    }

    VkCommandBuffer beginFrame();
    void endFrame();
    void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

private:
    void createCommandBuffers();
    void freeCommandBuffers();
    void recreateSwapChain();

    Window& _window;
    Device& _device;
    std::unique_ptr<SwapChain> _swapChain;
    std::vector<VkCommandBuffer> _commandBuffers;

    uint32_t _currentImageIndex;
    int _currentFrameIndex;
    bool _isFrameStarted;
};
}  // namespace vge
