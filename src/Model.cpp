#include "Model.h"

#include <cassert>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include <chrono>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "Utils.h"

namespace std {
template<>
struct hash<vge::Model::Vertex> {
    size_t operator()(vge::Model::Vertex const& vertex) const { 
        size_t seed = 0;
        vge::Utils::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
        return seed;
    }
};
}

namespace vge {

Model::Model(Device& device, const Builder& builder)
    : _device{device} {
    createVertexBuffers(builder.vertices);
    createIndexBuffers(builder.indices);
}

Model::~Model() {
    
}

void Model::createVertexBuffers(const std::vector<Vertex> &vertices) {
    _vertexCount = static_cast<uint32_t>(vertices.size());
    assert(_vertexCount >= 3 && "Vertex count must be at least 3");

    VkDeviceSize bufferSize = sizeof(vertices[0]) * _vertexCount;
    uint32_t vertexSize = sizeof(vertices[0]);

    Buffer stagingBuffer{_device,
                         vertexSize,
                         _vertexCount,
                         VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

    
    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void*)vertices.data());

    _vertexBuffer =
        std::make_unique<Buffer>(_device,
                                 vertexSize,
                                 _vertexCount,
                                 VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    _device.copyBuffer(stagingBuffer.getBuffer(), _vertexBuffer->getBuffer(), bufferSize);
}

void Model::createIndexBuffers(const std::vector<uint32_t>& indices) {
    _indexCount = static_cast<uint32_t>(indices.size());
    _hasIndexBuffer = !indices.empty();

    if (!_hasIndexBuffer) {
        return;
    }

    uint32_t indexSize = sizeof(indices[0]);
    VkDeviceSize bufferSize = sizeof(indices[0]) * _indexCount;

    Buffer stagingBuffer{_device,
                         indexSize,
                         _indexCount,
                         VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void*)indices.data());

    _indexBuffer =
        std::make_unique<Buffer>(_device,
                                 indexSize,
                                 _indexCount,
                                 VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    _device.copyBuffer(stagingBuffer.getBuffer(), _indexBuffer->getBuffer(), bufferSize);
}

void Model::draw(VkCommandBuffer commandBuffer) { 
    _hasIndexBuffer ? vkCmdDrawIndexed(commandBuffer, _indexCount, 1, 0, 0, 0)
                    : vkCmdDraw(commandBuffer, _vertexCount, 1, 0, 0);
}

std::unique_ptr<Model> Model::createModelFromFile(Device& device, const std::string_view& path) {
    Builder builder{};

    auto start = std::chrono::high_resolution_clock::now();
    builder.loadModel(path);
    auto elapsed = std::chrono::duration<float, std::chrono::seconds::period>(
                         std::chrono::high_resolution_clock::now() - start)
                         .count();

    std::cout << "Vertex count: " << builder.vertices.size() << '\n';
    std::cout << "Took: " << elapsed << "s\n";

    return std::make_unique<Model>(device, builder);
}

void Model::bind(VkCommandBuffer commandBuffer) {
    VkBuffer buffers[] = {_vertexBuffer->getBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

    if (_hasIndexBuffer) {
        vkCmdBindIndexBuffer(commandBuffer, _indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
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
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

    attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
    attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
    attributeDescriptions.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
    attributeDescriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)});

    return attributeDescriptions;
}

void Model::Builder::loadModel(const std::string_view& path) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn, error;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &error, path.data())) {
        throw std::runtime_error(warn + error);
    }

    vertices.clear();
    indices.clear();

    std::unordered_map<Vertex, uint32_t> uniqueVertices;

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            if (index.vertex_index >= 0) {
                vertex.position = {
                    attrib.vertices[3 * index.vertex_index],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2],
                };

                vertex.color = {
                    attrib.colors[3 * index.vertex_index],
                    attrib.colors[3 * index.vertex_index + 1],
                    attrib.colors[3 * index.vertex_index + 2],
                };
            }

            if (index.normal_index >= 0) {
                vertex.normal = {
                    attrib.normals[3 * index.normal_index],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2],
                };
            }

            if (index.texcoord_index >= 0) {
                vertex.uv = {
                    attrib.texcoords[2 * index.texcoord_index],
                    attrib.texcoords[2 * index.texcoord_index + 1],
                };
            }

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(std::move(vertex));
            }

            indices.push_back(uniqueVertices[vertex]);
        }

    }
}

}  // namespace vge