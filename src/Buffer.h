#pragma once

#include "Device.h"

namespace vge {
class Buffer {
public:
    Buffer(Device& device,
              VkDeviceSize instanceSize,
              uint32_t instanceCount,
              VkBufferUsageFlags usageFlags,
              VkMemoryPropertyFlags memoryPropertyFlags,
              VkDeviceSize minOffsetAlignment = 1);
    ~Buffer();

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    void unmap();

    void writeToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkDescriptorBufferInfo descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

    void writeToIndex(void* data, int index);
    VkResult flushIndex(int index);
    VkDescriptorBufferInfo descriptorInfoForIndex(int index);
    VkResult invalidateIndex(int index);

    inline VkBuffer getBuffer() const { return _buffer; }
    inline void* getMappedMemory() const { return _mappedMemory; }
    inline uint32_t getInstanceCount() const { return _instanceCount; }
    inline VkDeviceSize getInstanceSize() const { return _instanceSize; }
    inline VkDeviceSize getAlignmentSize() const { return _instanceSize; }
    inline VkBufferUsageFlags getUsageFlags() const { return _usageFlags; }
    inline VkMemoryPropertyFlags getMemoryPropertyFlags() const { return _memoryPropertyFlags; }
    inline VkDeviceSize getBufferSize() const { return _bufferSize; }

private:
    static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

    Device& _device;
    void* _mappedMemory = nullptr;
    VkBuffer _buffer = VK_NULL_HANDLE;
    VkDeviceMemory _memory = VK_NULL_HANDLE;

    VkDeviceSize _bufferSize;
    uint32_t _instanceCount;
    VkDeviceSize _instanceSize;
    VkDeviceSize _alignmentSize;
    VkBufferUsageFlags _usageFlags;
    VkMemoryPropertyFlags _memoryPropertyFlags;
};
}