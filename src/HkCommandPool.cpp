//
// Created by pach on 1/23/24.
//

#include "HkCommandPool.h"

HkCommandPool::HkCommandPool(HkDevice *pDevice) {
    this->pDevice = pDevice;
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
}
