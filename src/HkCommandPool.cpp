//
// Created by pach on 1/23/24.
//

#include "HkCommandPool.h"

HkCommandPool::HkCommandPool(HkDevice *pDevice, HkSwapchain *pSwapchain, HkGraphicPipeline *pGraphicPipeline) {
    this->pDevice = pDevice;
    this->pSwapchain = pSwapchain;
    this->pGraphicPipeline = pGraphicPipeline;
}

void HkCommandPool::createCommandPool() {
    if(pCommandPool) free(pCommandPool);
    pCommandPool = new VkCommandPool {};

    if(vkCreateCommandPool(*pDevice->getDevice(), &commandPoolInfo, nullptr, pCommandPool) != VK_SUCCESS){
        throw std::runtime_error("failed to create command pool");
    }
}

void HkCommandPool::fillDefaultCreateInfo() {
    commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolInfo.queueFamilyIndex = pDevice->getQueueFamilyIndices()->graphicFamily.value();

    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
}

void HkCommandPool::createCommandBuffers() {
    commandBuffers.resize(pSwapchain->getSwapchainImages()->size());

    allocateInfo.commandBufferCount = pSwapchain->getSwapchainImages()->size();
    allocateInfo.commandPool = *pCommandPool;

    if(vkAllocateCommandBuffers(*pDevice->getDevice(), &allocateInfo, commandBuffers.data()) != VK_SUCCESS){
        throw std::runtime_error("failed to create command buffers");
    }
}

std::vector<VkCommandBuffer> *HkCommandPool::getCommandBuffers() {
    return &commandBuffers;
}

HkGraphicPipeline *HkCommandPool::getGraphicPipeline() {
    return pGraphicPipeline;
}

HkSwapchain *HkCommandPool::getSwapchain() {
    return pSwapchain;
}

VkCommandPool *HkCommandPool::getCommandPool() {
    return pCommandPool;
}
