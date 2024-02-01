//
// Created by pach on 1/26/24.
//

#ifndef HAKUREIENGINE_HKSYNCOBJECT_H
#define HAKUREIENGINE_HKSYNCOBJECT_H
#include <iostream>
#include <vulkan/vulkan.h>
#include <vector>
#include "HkDevice.h"

class HkSyncObject {
private:
    HkDevice* pDevice;
public:
    HkSyncObject(HkDevice* device);

    std::vector<VkFence> gpuDoneFence;
    std::vector<VkSemaphore> imageAvailableSemaphore;
    std::vector<VkSemaphore> renderFinishedSemaphore;

public:
    void initSyncObjs(size_t size, VkDevice &device);

    void cleanup();
};


#endif //HAKUREIENGINE_HKSYNCOBJECT_H
