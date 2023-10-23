#pragma once

#include "Window.h"

#include <string>
#include <vector>
#include <optional>

namespace vge {

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

class Device {
public:
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    Device(Window& window);
    ~Device();

    // Not copyable or movable
    Device(const Device&) = delete;
    Device &operator=(const Device&) = delete;
    Device(Device&&) = delete;
    Device &operator=(Device&&) = delete;

    inline VkCommandPool getCommandPool() const { return _commandPool; }
    inline VkDevice getVkDevice() const { return _device; }
    inline VkSurfaceKHR getSurface() const { return _surface; }
    inline VkQueue getGraphicsQueue() const { return _graphicsQueue; }
    inline VkQueue getPresentQueue() const { return _presentQueue; }

    SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(_physicalDevice); }
    QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(_physicalDevice); }

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates,
                                 VkImageTiling tiling,
                                 VkFormatFeatureFlags features);

    // Buffer Helper Functions
    void createBuffer(VkDeviceSize size,
                      VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties,
                      VkBuffer &buffer,
                      VkDeviceMemory &bufferMemory);
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void copyBufferToImage(VkBuffer buffer,
                           VkImage image,
                           uint32_t _width,
                           uint32_t _height,
                           uint32_t layerCount);

    void createImageWithInfo(const VkImageCreateInfo &imageInfo,
                             VkMemoryPropertyFlags properties,
                             VkImage &image,
                             VkDeviceMemory &imageMemory);

    VkPhysicalDeviceProperties properties;

private:
    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createCommandPool();

    // helper functions
    bool isDeviceSuitable(VkPhysicalDevice device);
    std::vector<const char *> getRequiredExtensions();
    bool checkValidationLayerSupport();
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
    void hasGflwRequiredInstanceExtensions();
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

private:
    VkInstance _instance;
    VkDebugUtilsMessengerEXT _debugMessenger;
    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
    Window& _window;
    VkCommandPool _commandPool;

    VkDevice _device;
    VkSurfaceKHR _surface;
    VkQueue _graphicsQueue;
    VkQueue _presentQueue;

    const std::vector<const char *> _validationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char *> _deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};

}  // namespace vge
