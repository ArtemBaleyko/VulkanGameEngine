#pragma once

#include <vulkan/vulkan.h>

#include "Camera.h"
#include "GameObject.h"

namespace vge {
struct FrameInfo {
    int frameIndex;
    float frameTime;
    VkCommandBuffer commandBuffer;
    Camera& camera;
    VkDescriptorSet globalDescriptorSet;
    GameObject::Map& gameObjects;
};
}  // namespace vge