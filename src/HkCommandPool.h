//
// Created by pach on 1/23/24.
//

#ifndef HAKUREIENGINE_HKCOMMANDPOOL_H
#define HAKUREIENGINE_HKCOMMANDPOOL_H
#include <vulkan/vulkan.h>
#include "HkDevice.h"
#include "HkSwapchain.h"

class HkCommandPool {
private:
    // TODO make this vulkan object pointer, have like chain structure
    //  example: it should be no need to have same pointer type on every
    //  class object.
    //  (HkCommandPool->HkSwapchain->framebuffer) [correct]
    //  (HkCommandPool->HkSwapchain->HkGraphicpipeline->renderPass) [correct]
    HkDevice* pDevice = nullptr;
    HkSwapchain* pSwapchain = nullptr;
    HkGraphicPipeline* pGraphicPipeline = nullptr;

    VkCommandPool* pCommandPool = nullptr;
    std::vector<VkCommandBuffer> commandBuffers;
public:
    VkCommandPoolCreateInfo commandPoolInfo{};
    VkCommandBufferAllocateInfo allocateInfo{};


    HkCommandPool(HkDevice *pDevice, HkSwapchain *pSwapchain, HkGraphicPipeline *pGraphicPipeline);
    /// fill all required vulkan create info struct by default value,
    /// make sure the pointer on constructor params have the required value
    void fillDefaultCreateInfo();

    void createCommandPool();
    void createCommandBuffers();

    std::vector<VkCommandBuffer>* getCommandBuffers();
    HkGraphicPipeline* getGraphicPipeline();
    HkSwapchain* getSwapchain();
    VkCommandPool* getCommandPool();
};


#endif //HAKUREIENGINE_HKCOMMANDPOOL_H
