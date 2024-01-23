//
// Created by pach on 1/23/24.
//

#ifndef HAKUREIENGINE_HKCOMMANDPOOL_H
#define HAKUREIENGINE_HKCOMMANDPOOL_H
#include <vulkan/vulkan.h>
#include "HkDevice.h"

class HkCommandPool {
private:
    HkDevice* pDevice = nullptr;

    VkCommandPool* pCommandPool = nullptr;
public:
    VkCommandPoolCreateInfo commandPoolInfo{};


    HkCommandPool(HkDevice* pDevice);
    /// fill all required vulkan create info struct by default value,
    /// make sure the pointer on constructor params have the required value
    void fillDefaultCreateInfo();

    void createCommandPool();
};


#endif //HAKUREIENGINE_HKCOMMANDPOOL_H
