#include "src/Hk.h"

// TODO do some decision based on used window system manager name
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

static Vertex<Vert2> vertices({
                               {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                               {{0.5f,  -0.5f}, {0.0f, 1.0f, 0.0f}},
                               {{0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}},
                               {{-0.5f, 0.5f},  {1.0f, 1.0f, 1.0f}}
                       });
static Vertex<uint16_t> indices({
                                 0, 1, 2, 2, 3, 0
                         });

ImguiVulkanGlfw imgui(&device, &imguiSurface);

int main() {
    // setting required extensions by device and instance, also setting required layers
    device.setRequiredLayers(&requiredLayers1);
    device.setRequiredInstanceExtensions(&requiredExtensionsXcb);
    device.setRequiredDeviceExtensions(&requiredDeviceExtensions1);
    device.createInstance();

    // its has same vulkan instance
    imguiDevice.setInstance(device.getInstance());

    // creating window needed variable along with VkSurface
    surface.createSurface(&device);
    imguiSurface.createSurface(100, 100, "imgui");

    // pick suitable physical device, queue family, and swapchain support details based on current surface
    if (!device.pickPhysicalDevice(VK_QUEUE_GRAPHICS_BIT, *surface.getSurface())) {
        spdlog::error("failed picking suitable physical device");
        return 0;
    }
    if (!imguiDevice.pickPhysicalDevice(VK_QUEUE_GRAPHICS_BIT, *imguiSurface.getSurface())) {
        spdlog::error("failed picking suitable physical device");
        return 0;
    }

    // creating logical device, based on previous value. also creating VkQueue for graphic and present
    device.createLogicalDevice();
    imguiDevice.createLogicalDevice();

    // creating another vulkan things, like swapchain, framebuffer, image, etc. in imgui way
    imgui.init();

    // setting desired value of present mode, for swapchain later
    swapchain.setPresentMode(VK_PRESENT_MODE_MAILBOX_KHR);

    // creating swapchain, and image
    swapchain.createSwapchain();
    // TODO make the class have struct create info (the struct not passed on parameter)
    // creating imageView
    swapchain.createImageViews(nullptr);

    // creating semaphores, and fence. the size same as the size of image
    syncObject.initSyncObjs(swapchain.getSwapchainImages()->size(), *device.getDevice());

    // fill struct members on class with default value
    graphicPipeline.fillDefaultCreateInfo();

    // creating pipeline layout
    graphicPipeline.createPipelineLayout();

    // creating renderPass based on previous value
    graphicPipeline.createRenderPass();

    // setting up shader stage things for creating graphicPipeline
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
    // creating graphic pipeline with this shader stage, and the rest with default value
    graphicPipeline.createGraphicsPipeline(shaderStageCreateInfo, 2);

    // creating swapchain framebuffer based on graphicPipeline class member value
    swapchain.createFramebuffers(&graphicPipeline);

    // fill struct members on class with default value
    commandPool.fillDefaultCreateInfo();

    // creating command pool
    commandPool.createCommandPool();

    // create command buffer with size same as swapchain images size
    commandPool.createCommandBuffers();

    // creating vertex buffer
    vertices.createVertexBuffer(*device.getPhysicalDevice(), *device.getDevice(), *commandPool.getCommandPool(),
                                *device.getGraphicQueue(),
                                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    // creating indices buffer
    indices.createVertexBuffer(*device.getPhysicalDevice(), *device.getDevice(), *commandPool.getCommandPool(),
                               *device.getGraphicQueue(),
                               VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

    bool showDemoWindow = true;
    uint32_t currentFrame = 0;
    ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 0.00f);

    // set loop func callback
    surface.setLoop([&]() {


        // imgui glfw poll event
        glfwPollEvents();

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

        vkResetCommandBuffer((*commandPool.getCommandBuffers())[currentFrame], 0);
        // TODO make this func inside commandPool class
        util::recordFrameBuffer(&commandPool, currentFrame, vertices.getBuffer(), indices.getBuffer(), indices.data.size(),
                                imageIndex);

        VkPipelineStageFlags flag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &(*commandPool.getCommandBuffers())[currentFrame];
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

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if(showDemoWindow){
            ImGui::ShowDemoWindow(&showDemoWindow);
        }

        // imgui window
        {
            ImGui::Begin("Vulkan Debug");

            ImGui::Checkbox("Demo Window", &showDemoWindow);

            if(ImGui::CollapsingHeader("Vertices")) {
                static std::vector<std::vector<float>> vet2(vertices.data.size(), std::vector<float>(2));
                static std::vector<std::vector<float>> col2(vertices.data.size(), std::vector<float>(3));
                for (int i = 0; i < vertices.data.size(); i++) {
                    ImGui::PushID(i);
                    vet2[i][0] = vertices.data[i].location.x;
                    vet2[i][1] = vertices.data[i].location.y;
                    col2[i][0] = vertices.data[i].color.r;
                    col2[i][1] = vertices.data[i].color.g;
                    col2[i][2] = vertices.data[i].color.b;

                    ImGui::Text("Location: ");
                    ImGui::SameLine(80);
                    ImGui::InputFloat2("##", vet2[i].data());
                    ImGui::Text("\tColor");
                    ImGui::SameLine(80);
                    ImGui::ColorEdit3("##", col2[i].data());
                    ImGui::PopID();

                    vertices.data[i].location.x = vet2[i][0];
                    vertices.data[i].location.y = vet2[i][1];

                    vertices.data[i].color.r = col2[i][0];
                    vertices.data[i].color.g = col2[i][1];
                    vertices.data[i].color.b = col2[i][2];
                }
            }
            if(ImGui::CollapsingHeader("Indices")){
                static std::vector<std::vector<int>> index(indices.data.size(), std::vector<int>(1));
                for(int i=0; i<indices.data.size(); i++){
                    ImGui::PushID(i);
                    index[i][0] = indices.data[i];

                    ImGui::Text("Index: ");
                    ImGui::SameLine(80);
                    ImGui::InputInt("##", index[i].data());
                    index[i][0] = abs(index[i][0]);;
                    ImGui::PopID();

                    indices.data[i] = index[i][0];
                }
            }

            if(ImGui::CollapsingHeader("Info")){
                ImGui::Text("Current swapchain extent:\t(%u, %u)", swapchain.getSwapExtent().width,
                            swapchain.getSwapExtent().height);
            }

            ImGui::End();
        }

        // imgui render draw data, adn then present it
        imgui.renderAndPresent(clearColor);

        // recreating vertex buffer
        vertices.createVertexBuffer(*device.getPhysicalDevice(), *device.getDevice(), *commandPool.getCommandPool(),
                                    *device.getGraphicQueue(),
                                    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        // recreating indices buffer
        indices.createVertexBuffer(*device.getPhysicalDevice(), *device.getDevice(), *commandPool.getCommandPool(),
                                   *device.getGraphicQueue(),
                                   VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

        currentFrame = (currentFrame + 1) % swapchain.getSwapchainImages()->size();
    });
    surface.run();


}


