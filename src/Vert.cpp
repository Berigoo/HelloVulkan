//
// Created by pach on 1/25/24.
//

#include "Vert.h"

VkVertexInputBindingDescription Vert2::getBinding() {
    VkVertexInputBindingDescription description{};
    description.binding = 0;
    description.stride = sizeof(Vert2);
    description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return description;
}

std::array<VkVertexInputAttributeDescription, 2> Vert2::getAttribute() {
    std::array<VkVertexInputAttributeDescription, 2> description;

    description[0].binding = 0;
    description[0].location = 0;
    description[0].format = VK_FORMAT_R32G32_SFLOAT;
    description[0].offset = 0;

    description[1].binding = 0;
    description[1].location = 1;
    description[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    description[1].offset = 8;

    return description;
}


template<class T>
Vertex<T>::Vertex(std::vector<T> vertices) {
    data = vertices;
}

template<class T>
void Vertex<T>::createBuffer(VkPhysicalDevice physicalDevice, VkDevice device, int usageFlag, int memPropFlag, VkBuffer &outBuffer,
                             VkDeviceMemory &outBufferMemory) {
    VkResult res;
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.usage = usageFlag;
    bufferInfo.size = sizeof(data[0]) * data.size();
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    res = vkCreateBuffer(device, &bufferInfo, nullptr, &outBuffer);
    if(res != VK_SUCCESS) throw std::runtime_error("failed to create buffer");

    VkMemoryRequirements memReq{};
    vkGetBufferMemoryRequirements(device, outBuffer, &memReq);

    VkMemoryAllocateInfo memAllocateInfo{};
    memAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocateInfo.allocationSize = memReq.size;
    memAllocateInfo.memoryTypeIndex = util::findMemoryTypeIndex(physicalDevice, memReq.memoryTypeBits, memPropFlag);

    res = vkAllocateMemory(device, &memAllocateInfo, nullptr, &outBufferMemory);
    if(res != VK_SUCCESS) throw std::runtime_error("failed to allocate memory");

    vkBindBufferMemory(device, outBuffer, outBufferMemory, 0);
}

template<class T>
void Vertex<T>::createVertexBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkCommandPool commandPool,
                                   VkQueue &queue,
                                   VkBuffer &outVertexBuffer, VkDeviceMemory &outVertexBufferMemory, int usageFlag) {
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemoryBuffer;

    createBuffer(physicalDevice, device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, stagingBuffer, stagingMemoryBuffer);

    size_t size = sizeof(data[0]) * data.size();
    void* pData;
    vkMapMemory(device, stagingMemoryBuffer, 0, size, 0, &pData);
    memcpy(pData, data.data(), size);
    vkUnmapMemory(device, stagingMemoryBuffer);

    createBuffer(physicalDevice, device, usageFlag, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, outVertexBuffer, outVertexBufferMemory);

    copyBuffer(stagingBuffer, outVertexBuffer, commandPool, device, queue);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingMemoryBuffer, nullptr);
}

template<class T>
void Vertex<T>::copyBuffer(VkBuffer &src, VkBuffer &dst, VkCommandPool &commandPool, VkDevice &device, VkQueue &queue) {
    VkResult res;
    size_t size = sizeof(data[0]) * data.size();

    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.commandBufferCount = 1;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    VkCommandBuffer commandBuffer;
    res = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer);
    if(res != VK_SUCCESS) throw std::runtime_error("failed to allocate command buffer");

    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

    VkBufferCopy bufferCopy{};
    bufferCopy.srcOffset = 0;
    bufferCopy.dstOffset = 0;
    bufferCopy.size = size;

    vkCmdCopyBuffer(commandBuffer, src, dst, 1, &bufferCopy);
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}