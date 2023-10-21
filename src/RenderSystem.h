#pragma once

#include <memory>
#include <vector>

#include "Device.h"
#include "GameObject.h"
#include "Pipeline.h"

namespace vge {
class RenderSystem {
public:
    RenderSystem(Device& device, VkRenderPass renderPass);
    ~RenderSystem();

    RenderSystem(const RenderSystem&) = delete;
    RenderSystem& operator=(const RenderSystem&) = delete;

    void renderGameObjects(VkCommandBuffer commandBuffer, const std::vector<GameObject>& gameObjects);

private:
    void createPipelineLayout();
    void createPipeline(VkRenderPass renderPass);

private:
    Device& _device;

    std::unique_ptr<Pipeline> _pipeline;
    VkPipelineLayout _pipelineLayout;
};
}  // namespace vge