#include "Model.h"

#include <cassert>
#include <cstring>

namespace vge {
Model::Model(Device& device, const std::vector<Vertex>& verticies)
    : _device(device) {
    createVertexBuffers(verticies);
}

Model::~Model() {
    vkDestroyBuffer(_device.getVkDevice(), _vertexBuffer, nullptr);
    vkFreeMemory(_device.getVkDevice(), _vertexBufferMemory, nullptr);
}

void Model::draw(VkCommandBuffer commandBuffer) { vkCmdDraw(commandBuffer, _vertexCount, 1, 0, 0); }

void Model::bind(VkCommandBuffer commandBuffer) {
    VkBuffer buffers[] = { _vertexBuffer };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
}

void Model::createVertexBuffers(const std::vector<Vertex>& verticies) {
    _vertexCount = verticies.size();
    assert(_vertexCount >= 3 && "Sould be at least 3 verticies");

    VkDeviceSize bufferSize = sizeof(verticies[0]) * _vertexCount;

    _device.createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         _vertexBuffer, _vertexBufferMemory);

    void* data;
    vkMapMemory(_device.getVkDevice(), _vertexBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, verticies.data(), bufferSize);
    vkUnmapMemory(_device.getVkDevice(), _vertexBufferMemory);
}

std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescriptions() {
    std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
    bindingDescriptions[0].binding = 0;
    bindingDescriptions[0].stride = sizeof(Vertex);
    bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions() {
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, position);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    return attributeDescriptions;
}

}  // namespace vge