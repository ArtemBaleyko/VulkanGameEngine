#pragma once

#include "Camera.h"
#include "Device.h"
#include "GameObject.h"
#include "Pipeline.h"
#include "FrameInfo.h"

#include <memory>
#include <vector>

namespace vge {
class RenderSystem {
public:
    RenderSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~RenderSystem();

    RenderSystem(const RenderSystem &) = delete;
    RenderSystem &operator=(const RenderSystem &) = delete;

    void renderGameObjects(FrameInfo& frameInfo, std::vector<GameObject>& gameObjects);

private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

private:
    Device& _device;

    std::unique_ptr<Pipeline> _pipeline;
    VkPipelineLayout _pipelineLayout;
};
}  // namespace vge
