#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <string>
#include <vector>

#include "Device.h"

namespace vge {

class SwapChain {
public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    SwapChain(Device &deviceRef, VkExtent2D windowExtent);
    SwapChain(Device &deviceRef, VkExtent2D windowExtent, std::shared_ptr<SwapChain> previous);
    ~SwapChain();

    SwapChain(const SwapChain &) = delete;
    void operator=(const SwapChain &) = delete;

    inline VkFramebuffer getFrameBuffer(int index) { return _swapChainFramebuffers[index]; }
    inline VkRenderPass getRenderPass() { return _renderPass; }
    inline VkImageView getImageView(int index) { return _swapChainImageViews[index]; }
    inline size_t getImageCount() { return _swapChainImages.size(); }
    inline VkFormat getSwapChainImageFormat() { return _swapChainImageFormat; }
    inline VkExtent2D getSwapChainExtent() { return _swapChainExtent; }
    inline uint32_t getWidth() { return _swapChainExtent.width; }
    inline uint32_t getHeight() { return _swapChainExtent.height; }

    inline float extentAspectRatio() {
        return static_cast<float>(_swapChainExtent.width) / static_cast<float>(_swapChainExtent.height);
    }
    VkFormat findDepthFormat();

    VkResult acquireNextImage(uint32_t *imageIndex);
    VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

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
    VkExtent2D _swapChainExtent;

    std::vector<VkFramebuffer> _swapChainFramebuffers;
    VkRenderPass _renderPass;

    std::vector<VkImage> _depthImages;
    std::vector<VkDeviceMemory> _depthImageMemorys;
    std::vector<VkImageView> _depthImageViews;
    std::vector<VkImage> _swapChainImages;
    std::vector<VkImageView> _swapChainImageViews;

    Device &_device;
    VkExtent2D _windowExtent;

    VkSwapchainKHR _swapChain;

    std::vector<VkSemaphore> _imageAvailableSemaphores;
    std::vector<VkSemaphore> _renderFinishedSemaphores;
    std::vector<VkFence> _inFlightFences;
    std::vector<VkFence> _imagesInFlight;
    size_t _currentFrame = 0;

    std::shared_ptr<SwapChain> _oldSwapChain;
};

}  // namespace vge