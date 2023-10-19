#pragma once

#include <memory>
#include <vector>

#include "Device.h"
#include "Pipeline.h"
#include "SwapChain.h"
#include "Window.h"

namespace vge {
class Application {
public:
    Application();
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void run();

private:
    void createPipelineLayout();
    void createPipeline();
    void createCommandBuffers();
    void drawFrame();

private:
    Window _window{"Vulkan Game Engine", 800, 600};
    Device _device{_window};
    SwapChain _swapChain{_device, _window.getExtent()};
    std::unique_ptr<Pipeline> _pipeline;
    VkPipelineLayout _pipelineLayout;
    std::vector<VkCommandBuffer> _commandBuffers;
};
}  // namespace vge