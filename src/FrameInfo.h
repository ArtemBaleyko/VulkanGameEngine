#pragma once

#include <vulkan/vulkan.h>

#include "Camera.h"

namespace vge {
struct FrameInfo {
    int frameIndex;
    float frameTime;
    VkCommandBuffer commandBuffer;
    Camera& camera;
    VkDescriptorSet globalDescriptorSet;
};
}  // namespace vge