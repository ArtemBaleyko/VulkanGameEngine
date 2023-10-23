#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "Device.h"

namespace vge {
class DescriptorSetLayout {
public:
    class Builder {
    public:
        Builder(Device &device)
            : _device{device} {}

        Builder& addBinding(uint32_t binding,
                            VkDescriptorType descriptorType,
                            VkShaderStageFlags stageFlags,
                            uint32_t count = 1);
        std::unique_ptr<DescriptorSetLayout> build() const;

    private:
        Device &_device;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> _bindings{};
    };

    DescriptorSetLayout(Device &lveDevice,
                           std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
    ~DescriptorSetLayout();
    DescriptorSetLayout(const DescriptorSetLayout &) = delete;
    DescriptorSetLayout &operator=(const DescriptorSetLayout &) = delete;

    VkDescriptorSetLayout getDescriptorSetLayout() const { return _descriptorSetLayout; }

private:
    Device& _device;
    VkDescriptorSetLayout _descriptorSetLayout;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> _bindings;

    friend class DescriptorWriter;
};

class DescriptorPool {
public:
    class Builder {
    public:
        Builder(Device &device)
            : _device{device} {}

        Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
        Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
        Builder& setMaxSets(uint32_t count);
        std::unique_ptr<DescriptorPool> build() const;

    private:
        Device &_device;
        std::vector<VkDescriptorPoolSize> _poolSizes{};
        uint32_t _maxSets = 1000;
        VkDescriptorPoolCreateFlags _poolFlags = 0;
    };

    DescriptorPool(Device &device,
                      uint32_t maxSets,
                      VkDescriptorPoolCreateFlags poolFlags,
                      const std::vector<VkDescriptorPoolSize> &poolSizes);
    ~DescriptorPool();
    DescriptorPool(const DescriptorPool &) = delete;
    DescriptorPool &operator=(const DescriptorPool &) = delete;

    bool allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout,
                            VkDescriptorSet &descriptor) const;

    void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

    void resetPool();

private:
    Device &_device;
    VkDescriptorPool _descriptorPool;

    friend class DescriptorWriter;
};

class DescriptorWriter {
public:
    DescriptorWriter(DescriptorSetLayout &setLayout, DescriptorPool &pool);

    DescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
    DescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

    bool build(VkDescriptorSet &set);
    void overwrite(VkDescriptorSet &set);

private:
    DescriptorSetLayout& _setLayout;
    DescriptorPool& _pool;
    std::vector<VkWriteDescriptorSet> _writes;
};
}