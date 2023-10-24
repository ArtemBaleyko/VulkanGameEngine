#pragma once

#include <memory>
#include <vector>

#include "Camera.h"
#include "Device.h"
#include "FrameInfo.h"
#include "GameObject.h"
#include "Pipeline.h"

namespace vge {
class PointLightSystem {
public:
    PointLightSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~PointLightSystem();

    PointLightSystem(const PointLightSystem &) = delete;
    PointLightSystem &operator=(const PointLightSystem &) = delete;

    void render(FrameInfo &frameInfo);

private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

private:
    Device &_device;

    std::unique_ptr<Pipeline> _pipeline;
    VkPipelineLayout _pipelineLayout;
};
}  // namespace vge
