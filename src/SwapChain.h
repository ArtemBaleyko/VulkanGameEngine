#pragma once

#include "Device.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <string>
#include <vector>

namespace vge {

class SwapChain {
public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    SwapChain(Device &deviceRef, VkExtent2D windowExtent);
    SwapChain(Device &deviceRef, VkExtent2D windowExtent, std::shared_ptr<SwapChain> previous);

    ~SwapChain();

    SwapChain(const SwapChain &) = delete;
    SwapChain &operator=(const SwapChain &) = delete;

    inline VkFramebuffer getFrameBuffer(int index) const { return _swapChainFramebuffers[index]; }
    inline VkRenderPass getRenderPass() const { return _renderPass; }
    inline VkImageView getImageView(int index) const { return _swapChainImageViews[index]; }
    inline size_t imageCount() const { return _swapChainImages.size(); }
    inline VkFormat getSwapChainImageFormat() const { return _swapChainImageFormat; }
    inline VkExtent2D getSwapChainExtent() const { return _swapChainExtent; }
    inline uint32_t width() const { return _swapChainExtent.width; }
    inline uint32_t height() const { return _swapChainExtent.height; }

    inline float extentAspectRatio() const {
        return static_cast<float>(_swapChainExtent.width) / static_cast<float>(_swapChainExtent.height);
    }

    VkFormat findDepthFormat();

    VkResult acquireNextImage(uint32_t *imageIndex);
    VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

    inline bool compareSwapFormats(const SwapChain &swapChain) const {
        return swapChain._swapChainDepthFormat == _swapChainDepthFormat &&
               swapChain._swapChainImageFormat == _swapChainImageFormat;
    }

private:
    void init();
    void createSwapChain();
    void createImageViews();
    void createDepthResources();
    void createRenderPass();
    void createFramebuffers();
    void createSyncObjects();

    // Helper functions
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

private:
    VkFormat _swapChainImageFormat;
    VkFormat _swapChainDepthFormat;
    VkExtent2D _swapChainExtent;

    std::vector<VkFramebuffer> _swapChainFramebuffers;
    VkRenderPass _renderPass;

    std::vector<VkImage> _depthImages;
    std::vector<VkDeviceMemory> _depthImageMemorys;
    std::vector<VkImageView> _depthImageViews;
    std::vector<VkImage> _swapChainImages;
    std::vector<VkImageView> _swapChainImageViews;

    Device& _device;
    VkExtent2D _windowExtent;

    VkSwapchainKHR _swapChain;
    std::shared_ptr<SwapChain> _oldSwapChain;

    std::vector<VkSemaphore> _imageAvailableSemaphores;
    std::vector<VkSemaphore> _renderFinishedSemaphores;
    std::vector<VkFence> _inFlightFences;
    std::vector<VkFence> _imagesInFlight;
    size_t _currentFrame = 0;
};

}  // namespace vge
