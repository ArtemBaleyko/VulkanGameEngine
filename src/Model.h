#pragma once

#include "Device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace vge {
class Model {
public:
    struct Vertex {
        glm::vec3 position{};
        glm::vec3 color{};

        static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
    };

    struct Builder {
        std::vector<Vertex> vertices{};
        std::vector<uint32_t> indices{};
    };

    Model(Device& device, const Builder& builder);
    ~Model();

    Model(const Model&) = delete;
    Model &operator=(const Model&) = delete;

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

private:
    void createVertexBuffers(const std::vector<Vertex> &vertices);
    void createIndexBuffers(const std::vector<uint32_t>& indices);

private:
    Device& _device;

    VkBuffer _vertexBuffer;
    VkDeviceMemory _vertexBufferMemory;
    uint32_t _vertexCount;

    bool _hasIndexBuffer;
    VkBuffer _indexBuffer;
    VkDeviceMemory _indexBufferMemory;
    uint32_t _indexCount;
};
}  // namespace vge
