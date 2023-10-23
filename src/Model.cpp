#include "Model.h"

#include <cassert>
#include <cstring>

namespace vge {

Model::Model(Device& device, const Builder& builder)
    : _device{device} {
    createVertexBuffers(builder.vertices);
    createIndexBuffers(builder.indices);
}

Model::~Model() {
    vkDestroyBuffer(_device.getVkDevice(), _vertexBuffer, nullptr);
    vkFreeMemory(_device.getVkDevice(), _vertexBufferMemory, nullptr);

    if (_hasIndexBuffer) {
        vkDestroyBuffer(_device.getVkDevice(), _indexBuffer, nullptr);
        vkFreeMemory(_device.getVkDevice(), _indexBufferMemory, nullptr);
    }
}

void Model::createVertexBuffers(const std::vector<Vertex> &vertices) {
    _vertexCount = static_cast<uint32_t>(vertices.size());
    assert(_vertexCount >= 3 && "Vertex count must be at least 3");

    VkDeviceSize bufferSize = sizeof(vertices[0]) * _vertexCount;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    _device.createBuffer(bufferSize,
                         VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         stagingBuffer,
                         stagingBufferMemory);

    void *data;
    vkMapMemory(_device.getVkDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(_device.getVkDevice(), stagingBufferMemory);

    _device.createBuffer(bufferSize,
                         VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                         _vertexBuffer,
                         _vertexBufferMemory);

    _device.copyBuffer(stagingBuffer, _vertexBuffer, bufferSize);

    vkDestroyBuffer(_device.getVkDevice(), stagingBuffer, nullptr);
    vkFreeMemory(_device.getVkDevice(), stagingBufferMemory, nullptr);
}

void Model::createIndexBuffers(const std::vector<uint32_t>& indices) {
    _indexCount = static_cast<uint32_t>(indices.size());
    _hasIndexBuffer = !indices.empty();

    if (!_hasIndexBuffer) {
        return;
    }

    VkDeviceSize bufferSize = sizeof(indices[0]) * _indexCount;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    _device.createBuffer(bufferSize,
                         VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         stagingBuffer,
                         stagingBufferMemory);

    void* data;
    vkMapMemory(_device.getVkDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(_device.getVkDevice(), stagingBufferMemory);

    _device.createBuffer(bufferSize,
                         VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                         _indexBuffer,
                         _indexBufferMemory);

    _device.copyBuffer(stagingBuffer, _indexBuffer, bufferSize);

    vkDestroyBuffer(_device.getVkDevice(), stagingBuffer, nullptr);
    vkFreeMemory(_device.getVkDevice(), stagingBufferMemory, nullptr);
}

void Model::draw(VkCommandBuffer commandBuffer) { 
    _hasIndexBuffer ? vkCmdDrawIndexed(commandBuffer, _indexCount, 1, 0, 0, 0)
                    : vkCmdDraw(commandBuffer, _vertexCount, 1, 0, 0);
}

void Model::bind(VkCommandBuffer commandBuffer) {
    VkBuffer buffers[] = {_vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

    if (_hasIndexBuffer) {
        vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
    }
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