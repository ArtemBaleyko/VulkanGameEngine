#pragma once

#include <vulkan/vulkan.h>

#include "Camera.h"
#include "GameObject.h"

namespace vge {

const int MAX_LIGHTS = 10;

struct PointLight {
    glm::vec4 position{};
    glm::vec4 color{};
};

struct GlobalUbo {
    glm::mat4 projection{1.0f};
    glm::mat4 view{1.0f};
    glm::vec4 ambientLightColor{1.0f, 1.0f, 1.0f, 0.2f};
    PointLight pointLights[MAX_LIGHTS];
    int numLights;
};

struct FrameInfo {
    int frameIndex;
    float frameTime;
    VkCommandBuffer commandBuffer;
    Camera& camera;
    VkDescriptorSet globalDescriptorSet;
    GameObject::Map& gameObjects;
};
}  // namespace vge