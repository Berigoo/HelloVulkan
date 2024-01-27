//
// Created by pach on 1/27/24.
//

#include "ImguiVulkanGlfw.h"

void ImguiVulkanGlfw::init() {
    int w, h;
    glfwGetFramebufferSize(dynamic_cast<GlfwSurface*>(pSurface)->getWindow(), &w, &h);
    ImGui_ImplVulkanH_Window* wd = &windowData;
    setupVulkanWindow(&windowData, pDevice, pSurface, w, h, minImageCount);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO(); (void)io;
    io.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(dynamic_cast<GlfwSurface*>(pSurface)->getWindow(), true);
    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.Instance = pDevice->getInstance();
    initInfo.PhysicalDevice = *pDevice->getPhysicalDevice();
    initInfo.Device = *pDevice->getDevice();
    initInfo.QueueFamily = *pDevice->getQueueFamilyIndices()->graphicFamily;
    initInfo.Queue = *pDevice->getGraphicQueue();
    initInfo.PipelineCache = VK_NULL_HANDLE;
    VkDescriptorPool descriptorPool1;
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
    VkDescriptorPoolSize poolSize[] = {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    descriptorPoolCreateInfo.maxSets = 1;
    descriptorPoolCreateInfo.poolSizeCount = (uint32_t) IM_ARRAYSIZE(poolSize);
    descriptorPoolCreateInfo.pPoolSizes = poolSize;
    if(vkCreateDescriptorPool(*pDevice->getDevice(), &descriptorPoolCreateInfo, nullptr, &descriptorPool1) != VK_SUCCESS){
        throw std::runtime_error("ImGui failed to create descriptor pool");
    }
    initInfo.DescriptorPool = descriptorPool1;
    initInfo.Subpass = 0;
    initInfo.MinImageCount = minImageCount;
    initInfo.ImageCount = windowData.ImageCount;
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    initInfo.Allocator = nullptr;
    if(ImGui_ImplVulkan_Init(&initInfo, windowData.RenderPass)) spdlog::debug("imgui vulkan init success");

}

void ImguiVulkanGlfw::setupVulkanWindow(ImGui_ImplVulkanH_Window* wd, HkDevice* pDevice, HkSurface* pSurface, int width, int height, uint32_t
minImageCount) {
    VkPhysicalDevice physicalDevice = *pDevice->getPhysicalDevice();

    windowData.Surface = *pSurface->getSurface();

    // Check for WSI support
    VkBool32 res;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, pDevice->getQueueFamilyIndices()->graphicFamily.value(), windowData.Surface, &res);
    if (res != VK_TRUE)
    {
        fprintf(stderr, "Error no WSI support on physical device 0\n");
        exit(-1);
    }

    // Select Surface Format
    const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
    const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    windowData.SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(physicalDevice, windowData.Surface, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

    // Select Present Mode
#ifdef IMGUI_UNLIMITED_FRAME_RATE
    VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
    VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
#endif
    windowData.PresentMode = ImGui_ImplVulkanH_SelectPresentMode(physicalDevice, windowData.Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));
    //printf("[vulkan] Selected PresentMode = %d\n", windowData.PresentMode);

    // Create SwapChain, RenderPass, Framebuffer, etc.
    IM_ASSERT(minImageCount >= 2);
    ImGui_ImplVulkanH_CreateOrResizeWindow(pDevice->getInstance(), physicalDevice, *pDevice->getDevice(), wd, pDevice->getQueueFamilyIndices()->graphicFamily.value(),
                                           nullptr, width, height, minImageCount);
}

ImguiVulkanGlfw::ImguiVulkanGlfw(HkDevice *pDevice, HkSurface *pSurface) : ImguiBase(pDevice, pSurface) {

}

void ImguiVulkanGlfw::renderAndPresent(ImVec4 clearColor) {
    ImGui::Render();
    ImDrawData* drawData = ImGui::GetDrawData();
    const bool isMinimize = (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f);
    if(!isMinimize){
        windowData.ClearValue.color.float32[0] = clearColor.x * clearColor.w;
        windowData.ClearValue.color.float32[1] = clearColor.y * clearColor.w;
        windowData.ClearValue.color.float32[2] = clearColor.z * clearColor.w;
        windowData.ClearValue.color.float32[3] = clearColor.w;
        frameRender(drawData);
        framePresent();
    }
    resizeSwapchain();
}

void ImguiVulkanGlfw::frameRender(ImDrawData *drawData) {
    VkResult err;

    VkSemaphore image_acquired_semaphore  = windowData.FrameSemaphores[windowData.SemaphoreIndex].ImageAcquiredSemaphore;
    VkSemaphore render_complete_semaphore = windowData.FrameSemaphores[windowData.SemaphoreIndex].RenderCompleteSemaphore;
    err = vkAcquireNextImageKHR(*pDevice->getDevice(), windowData.Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &windowData.FrameIndex);
    if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
    {
        swapChainResizedPoll = true;
        return;
    }
    check_vk_result(err);

    ImGui_ImplVulkanH_Frame* fd = &windowData.Frames[windowData.FrameIndex];
    {
        err = vkWaitForFences(*pDevice->getDevice(), 1, &fd->Fence, VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
        check_vk_result(err);

        err = vkResetFences(*pDevice->getDevice(), 1, &fd->Fence);
        check_vk_result(err);
    }
    {
        err = vkResetCommandPool(*pDevice->getDevice(), fd->CommandPool, 0);
        check_vk_result(err);
        VkCommandBufferBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
        check_vk_result(err);
    }
    {
        VkRenderPassBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.renderPass = windowData.RenderPass;
        info.framebuffer = fd->Framebuffer;
        info.renderArea.extent.width = windowData.Width;
        info.renderArea.extent.height = windowData.Height;
        info.clearValueCount = 1;
        info.pClearValues = &windowData.ClearValue;
        vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
    }

    // Record dear imgui primitives into command buffer
    ImGui_ImplVulkan_RenderDrawData(drawData, fd->CommandBuffer);

    // Submit command buffer
    vkCmdEndRenderPass(fd->CommandBuffer);
    {
        VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &image_acquired_semaphore;
        info.pWaitDstStageMask = &wait_stage;
        info.commandBufferCount = 1;
        info.pCommandBuffers = &fd->CommandBuffer;
        info.signalSemaphoreCount = 1;
        info.pSignalSemaphores = &render_complete_semaphore;

        err = vkEndCommandBuffer(fd->CommandBuffer);
        check_vk_result(err);
        err = vkQueueSubmit(*pDevice->getGraphicQueue(), 1, &info, fd->Fence);
        check_vk_result(err);
    }
}

void ImguiVulkanGlfw::resizeSwapchain() {
    if(swapChainResizedPoll) {
        int width, height;
        glfwGetFramebufferSize(dynamic_cast<GlfwSurface *>(pSurface)->getWindow(), &width, &height);
        if (width > 0 && height > 0) {
            ImGui_ImplVulkan_SetMinImageCount(minImageCount);
            ImGui_ImplVulkanH_CreateOrResizeWindow(pDevice->getInstance(), *pDevice->getPhysicalDevice(),
                                                   *pDevice->getDevice(), &windowData,
                                                   pDevice->getQueueFamilyIndices()->graphicFamily.value(),
                                                   nullptr, width, height, minImageCount);
            windowData.FrameIndex = 0;
            swapChainResizedPoll = false;
        }
    }
}

void ImguiVulkanGlfw::check_vk_result(VkResult err) {
    if (err == 0)
        return;
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0)
        abort();
}

void ImguiVulkanGlfw::framePresent() {
    if (swapChainResizedPoll)
        return;
    VkSemaphore render_complete_semaphore = windowData.FrameSemaphores[windowData.SemaphoreIndex].RenderCompleteSemaphore;
    VkPresentInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &render_complete_semaphore;
    info.swapchainCount = 1;
    info.pSwapchains = &windowData.Swapchain;
    info.pImageIndices = &windowData.FrameIndex;
    VkResult err = vkQueuePresentKHR(*pDevice->getPresentQueue(), &info);
    if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
    {
        swapChainResizedPoll = true;
        return;
    }
    check_vk_result(err);
    windowData.SemaphoreIndex = (windowData.SemaphoreIndex + 1) % windowData.ImageCount;
}
