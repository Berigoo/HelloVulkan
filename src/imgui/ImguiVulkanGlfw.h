//
// Created by pach on 1/27/24.
//

#ifndef HAKUREIENGINE_IMGUIVULKANGLFW_H
#define HAKUREIENGINE_IMGUIVULKANGLFW_H
#include "ImguiBase.h"
#include "../GlfwSurface.h"

class ImguiVulkanGlfw : public ImguiBase{
private:
    ImGui_ImplVulkanH_Window windowData;
    bool swapChainResizedPoll = false;

    VkDescriptorPool descriptorPool;
    uint32_t minImageCount = 2;
public:
    ImguiVulkanGlfw(HkDevice* pDevice, HkSurface* pSurface);

    void init() override;
    void renderAndPresent(ImVec4 clearColor);
    void cleanup(VkInstance instance, VkDevice device, VkSurfaceKHR *surface);
private:
    void setupVulkanWindow(ImGui_ImplVulkanH_Window* wd, HkDevice* pDevice, HkSurface* pSurface, int width, int height, uint32_t minImageCount);
    void frameRender(ImDrawData *drawData);
    void framePresent();
    void resizeSwapchain();

    static void check_vk_result(VkResult err);
};


#endif //HAKUREIENGINE_IMGUIVULKANGLFW_H
