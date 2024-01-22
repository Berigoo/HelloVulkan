#include <iostream>
#include "src/Hk.h"

// TODO do some decision based on used window system manager name
//  not hard coded like this
//  maybe, every surface class must have its own must-needed extensions name
//  then the value pushed-back into device class required-extensions
std::vector<const char*> requiredLayers1 = {"VK_LAYER_KHRONOS_validation"};
std::vector<const char*> requiredExtensionsXcb = {VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_XCB_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
std::vector<const char*> requiredDeviceExtensions1 = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

int main() {
    HkDevice device;
    device.setRequiredLayers(&requiredLayers1);
    device.setRequiredInstanceExtensions(&requiredExtensionsXcb);
    device.setRequiredDeviceExtensions(&requiredDeviceExtensions1);
    device.createInstance();

    XcbSurface surface(500, 800, (XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK), (XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY));
    surface.createSurface(&device);

    if(!device.pickPhysicalDevice(VK_QUEUE_GRAPHICS_BIT, *surface.getSurface())){
        spdlog::error("failed picking suitable physical device");
        return 0;
    }
    device.createLogicalDevice();

    HkSwapchain swapchain(&device, &surface);
    swapchain.createSwapchain();
    swapchain.createImageViews(nullptr);

    HkGraphicPipeline graphicPipeline(&device, &swapchain);
    graphicPipeline.createPipelineLayout();
    graphicPipeline.createRenderPass();
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo[2] = {};
    shaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    const std::vector<char> vertModule = HkGraphicPipeline::readFile("/disk0/clionProject/hakureiEngine/shaders/out/vert.spv");
    shaderStageCreateInfo[0].module = graphicPipeline.createShaderModule(&vertModule);
    shaderStageCreateInfo[0].pName = "main";
    shaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    const std::vector<char> fragModule = HkGraphicPipeline::readFile("/disk0/clionProject/hakureiEngine/shaders/out/frag.spv");
    shaderStageCreateInfo[1].module = graphicPipeline.createShaderModule(&fragModule);
    shaderStageCreateInfo[1].pName = "main";
    graphicPipeline.createGraphicsPipeline(shaderStageCreateInfo, 2);
}
