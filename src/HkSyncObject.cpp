//
// Created by pach on 1/26/24.
//

#include "HkSyncObject.h"

void HkSyncObject::initSyncObjs(size_t size, VkDevice &device) {
    gpuDoneFence.resize(size);
    imageAvailableSemaphore.resize(size);
    renderFinishedSemaphore.resize(size);

    VkSemaphoreCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for(int i=0; i<size; i++){
        if(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphore[i]) != VK_SUCCESS){
            throw std::runtime_error("failed to create semaphores");
        }
        if(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphore[i]) != VK_SUCCESS){
            throw std::runtime_error("failed to create semaphores");
        }
        if(vkCreateFence(device, &fenceCreateInfo, nullptr, &gpuDoneFence[i]) != VK_SUCCESS){
            throw std::runtime_error("failed to create semaphores");
        }
    }
}

void HkSyncObject::cleanup() {
    for(int i=0; i<gpuDoneFence.size(); i++){
        vkDestroyFence(*pDevice->getDevice(), gpuDoneFence[i], nullptr);
    }
    for(int i=0; i<imageAvailableSemaphore.size(); i++){
        vkDestroySemaphore(*pDevice->getDevice(), imageAvailableSemaphore[i], nullptr);
    }
    for(int i=0; i<renderFinishedSemaphore.size(); i++){
        vkDestroySemaphore(*pDevice->getDevice(), renderFinishedSemaphore[i], nullptr);
    }
}

HkSyncObject::HkSyncObject(HkDevice *device) {
    this->pDevice = device;
}
