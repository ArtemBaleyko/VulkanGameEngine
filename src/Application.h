#pragma once

#include <memory>
#include <vector>

#include "Device.h"
#include "Model.h"
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
    void loadModels();
    void createPipelineLayout();
    void createPipeline();
    void createCommandBuffers();
    void freeCommandBuffers();
    void drawFrame();

    void recreateSwapChain();
    void recordCommandBuffer(int imageIndex);

private:
    Window _window{"Vulkan Game Engine", 800, 600};
    Device _device{_window};
    std::unique_ptr<SwapChain> _swapChain;
    std::unique_ptr<Pipeline> _pipeline;
    VkPipelineLayout _pipelineLayout;
    std::vector<VkCommandBuffer> _commandBuffers;
    std::unique_ptr<Model> _model;
};
}  // namespace vge