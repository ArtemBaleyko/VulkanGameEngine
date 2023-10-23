#pragma once

#include "Camera.h"
#include "Device.h"
#include "GameObject.h"
#include "Pipeline.h"

#include <memory>
#include <vector>

namespace vge {
class RenderSystem {
public:
    RenderSystem(Device &device, VkRenderPass renderPass);
    ~RenderSystem();

    RenderSystem(const RenderSystem &) = delete;
    RenderSystem &operator=(const RenderSystem &) = delete;

    void renderGameObjects(VkCommandBuffer commandBuffer,
                           std::vector<GameObject> &gameObjects,
                           const Camera &camera);

private:
    void createPipelineLayout();
    void createPipeline(VkRenderPass renderPass);

private:
    Device& _device;

    std::unique_ptr<Pipeline> _pipeline;
    VkPipelineLayout _pipelineLayout;
};
}  // namespace vge
