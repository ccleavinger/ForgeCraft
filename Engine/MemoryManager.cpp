#include "Engine/MemoryManager.h"

using namespace VoxelEngine;

MemoryManager::MemoryManager(vk::Device& device) {
    createAllocator(device);
}

MemoryManager::~MemoryManager() {
    vmaDestroyAllocator(m_allocator);
}

void MemoryManager::createAllocator(vk::Device& device) {
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_0;
    allocatorInfo.physicalDevice = device.physicalDevice().handle();
    allocatorInfo.device = device.handle();
    allocatorInfo.instance = device.instance().handle();

    if (vmaCreateAllocator(&allocatorInfo, &m_allocator) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create memory allocator");
    }
}