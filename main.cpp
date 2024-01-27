#include "src/Hk.h"

// TODO do some decision based on used window system manager name
//  not hard coded like this
//  maybe, every surface class must have its own must-needed extensions name
//  then the value pushed-back into device class required-extensions
std::vector<const char *> requiredLayers1 = {"VK_LAYER_KHRONOS_validation"};
std::vector<const char *> requiredExtensionsXcb = {VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_XCB_SURFACE_EXTENSION_NAME,
                                                   VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
std::vector<const char *> requiredDeviceExtensions1 = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

HkDevice device;
HkDevice imguiDevice;
XcbSurface surface(500, 800, (XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK),
                   (XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY));
GlfwSurface imguiSurface(&imguiDevice);
HkSyncObject syncObject;
HkSwapchain swapchain(&device, &surface, &syncObject);
HkGraphicPipeline graphicPipeline(&device, &swapchain);
HkCommandPool commandPool(&device, &swapchain, &graphicPipeline);

Vertex<Vert2> vertices({
                               {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                               {{0.5f,  -0.5f}, {0.0f, 1.0f, 0.0f}},
                               {{0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}},
                               {{-0.5f, 0.5f},  {1.0f, 1.0f, 1.0f}}
                       });
Vertex<uint16_t> indices({
                                 0, 1, 2, 2, 3, 0
                         });
VkBuffer vertexBuffer;
VkDeviceMemory vertexBufferMemory;
VkBuffer indicesBuffer;
VkDeviceMemory indicesBufferMemory;

ImguiVulkanGlfw imgui(&device, &imguiSurface);

int main() {
    device.setRequiredLayers(&requiredLayers1);
    device.setRequiredInstanceExtensions(&requiredExtensionsXcb);
    device.setRequiredDeviceExtensions(&requiredDeviceExtensions1);
    device.createInstance();

    uint32_t extCount;
    const char** exts = glfwGetRequiredInstanceExtensions(&extCount);
    std::vector<const char*> glfwExts(extCount);
    for(int i=0; i<extCount; i++)glfwExts[i] = exts[i];
    imguiDevice.setInstance(device.getInstance());


    surface.createSurface(&device);
    imguiSurface.init(100, 100, "imgui");

    if (!device.pickPhysicalDevice(VK_QUEUE_GRAPHICS_BIT, *surface.getSurface())) {
        spdlog::error("failed picking suitable physical device");
        return 0;
    }
    device.createLogicalDevice();

    if (!imguiDevice.pickPhysicalDevice(VK_QUEUE_GRAPHICS_BIT, *imguiSurface.getSurface())) {
        spdlog::error("failed picking suitable physical device");
        return 0;
    }
    imguiDevice.createLogicalDevice();

    imgui.init();

    swapchain.setPresentMode(VK_PRESENT_MODE_MAILBOX_KHR);
    swapchain.createSwapchain();
    // TODO make the class have struct create info (the struct not passed on parameter)
    swapchain.createImageViews(nullptr);

    syncObject.initSyncObjs(swapchain.getSwapchainImages()->size(), *device.getDevice());

    graphicPipeline.fillDefaultCreateInfo();
    graphicPipeline.createPipelineLayout();
    graphicPipeline.createRenderPass();
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo[2] = {};
    shaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    const std::vector<char> vertModule = HkGraphicPipeline::readFile(
            "/disk0/clionProject/hakureiEngine/shaders/out/vert.spv");
    shaderStageCreateInfo[0].module = graphicPipeline.createShaderModule(&vertModule);
    shaderStageCreateInfo[0].pName = "main";
    shaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    const std::vector<char> fragModule = HkGraphicPipeline::readFile(
            "/disk0/clionProject/hakureiEngine/shaders/out/frag.spv");
    shaderStageCreateInfo[1].module = graphicPipeline.createShaderModule(&fragModule);
    shaderStageCreateInfo[1].pName = "main";

    std::array<VkVertexInputAttributeDescription, 2> desc = Vert2::getAttribute();
    VkVertexInputBindingDescription bindingDescription = Vert2::getBinding();

    graphicPipeline.vertexInputInfo.vertexAttributeDescriptionCount = 2;
    graphicPipeline.vertexInputInfo.pVertexAttributeDescriptions = Vert2::getAttribute().data();
    graphicPipeline.vertexInputInfo.vertexBindingDescriptionCount = 1;
    graphicPipeline.vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    graphicPipeline.createGraphicsPipeline(shaderStageCreateInfo, 2);

    swapchain.createFramebuffers(&graphicPipeline);

    commandPool.fillDefaultCreateInfo();
    commandPool.createCommandPool();
    commandPool.createCommandBuffers();

    vertices.createVertexBuffer(*device.getPhysicalDevice(), *device.getDevice(), *commandPool.getCommandPool(),
                                *device.getGraphicQueue(), vertexBuffer, vertexBufferMemory,
                                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    indices.createVertexBuffer(*device.getPhysicalDevice(), *device.getDevice(), *commandPool.getCommandPool(),
                               *device.getGraphicQueue(), indicesBuffer, indicesBufferMemory,
                               VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

    bool showDemoWindow = true;
    uint32_t currentFrame = 0;
    ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    surface.setLoop([&]() {


        // imgui
        glfwPollEvents();

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if(showDemoWindow){
            ImGui::ShowDemoWindow(&showDemoWindow);
        }

        imgui.renderAndPresent(clearColor);


        // acquire image, render into image, present into surface
        vkDeviceWaitIdle(*device.getDevice());
        vkWaitForFences(*device.getDevice(), 1, &swapchain.getSyncObject()->gpuDoneFence[currentFrame], VK_TRUE,
                        UINT64_MAX);


        uint32_t imageIndex;
        VkResult res = vkAcquireNextImageKHR(*device.getDevice(), *swapchain.getSwapchain(), UINT64_MAX,
                                             swapchain.getSyncObject()->imageAvailableSemaphore[currentFrame],
                                             VK_NULL_HANDLE, &imageIndex);

        if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR || surface.windowResizedPoll) {
            VkSemaphoreCreateInfo semaphoreCreateInfo{};
            vkDestroySemaphore(*device.getDevice(), swapchain.getSyncObject()->imageAvailableSemaphore[currentFrame],
                               nullptr);
            semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            vkCreateSemaphore(*device.getDevice(), &semaphoreCreateInfo, nullptr,
                              &swapchain.getSyncObject()->imageAvailableSemaphore[currentFrame]);

            surface.windowResizedPoll = false;
            swapchain.recreateSwapchain(&graphicPipeline);
            return;
        } else if (res != VK_SUCCESS) {
            throw std::runtime_error("failed to acquire next image index");
        }

        vkResetFences(*device.getDevice(), 1, &swapchain.getSyncObject()->gpuDoneFence[currentFrame]);

        vkResetCommandBuffer(commandPool.getCommandBuffers()->data()[currentFrame], 0);
        // TODO make this func inside commandPool class
        util::recordFrameBuffer(&commandPool, currentFrame, &vertexBuffer, &indicesBuffer, indices.data.size(),
                                imageIndex);

        VkPipelineStageFlags flag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandPool.getCommandBuffers()->data()[currentFrame];
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &swapchain.getSyncObject()->imageAvailableSemaphore[currentFrame];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &swapchain.getSyncObject()->renderFinishedSemaphore[currentFrame];
        submitInfo.pWaitDstStageMask = &flag;

        res = vkQueueSubmit(*device.getGraphicQueue(), 1, &submitInfo,
                            swapchain.getSyncObject()->gpuDoneFence[currentFrame]);
        if (res != VK_SUCCESS) throw std::runtime_error("failed to submit to graphic queue");

        VkPresentInfoKHR presentInfoKhr{};
        presentInfoKhr.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfoKhr.waitSemaphoreCount = 1;
        presentInfoKhr.pWaitSemaphores = &swapchain.getSyncObject()->renderFinishedSemaphore[currentFrame];
        presentInfoKhr.swapchainCount = 1;
        presentInfoKhr.pSwapchains = swapchain.getSwapchain();
        presentInfoKhr.pImageIndices = &imageIndex;

        vkQueuePresentKHR(*device.getPresentQueue(), &presentInfoKhr);

        currentFrame = (currentFrame + 1) % swapchain.getSwapchainImages()->size();
    });
    surface.run();
}


