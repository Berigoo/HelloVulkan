#include "src/Hk.h"

// TODO do some decision based on used window system manager name
//  not hard coded like this
//  maybe, every surface class must have its own must-needed extensions name
//  then the value pushed-back into device class required-extensions
std::vector<const char*> requiredLayers1 = {"VK_LAYER_KHRONOS_validation"};
std::vector<const char*> requiredExtensionsXcb = {VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_XCB_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
std::vector<const char*> requiredDeviceExtensions1 = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

HkDevice device;
XcbSurface surface(500, 800, (XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK), (XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY));
HkSwapchain swapchain(&device, &surface);
HkGraphicPipeline graphicPipeline(&device, &swapchain);
HkCommandPool commandPool(&device, &swapchain, &graphicPipeline);

Vertex<Vert2> vertices({
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
});
Vertex<uint16_t> indices({
    0, 1, 2, 2, 3, 0
});
VkBuffer vertexBuffer;
VkDeviceMemory vertexBufferMemory;
VkBuffer indicesBuffer;
VkDeviceMemory indicesBufferMemory;

int main() {
    device.setRequiredLayers(&requiredLayers1);
    device.setRequiredInstanceExtensions(&requiredExtensionsXcb);
    device.setRequiredDeviceExtensions(&requiredDeviceExtensions1);
    device.createInstance();

    surface.createSurface(&device);

    if(!device.pickPhysicalDevice(VK_QUEUE_GRAPHICS_BIT, *surface.getSurface())){
        spdlog::error("failed picking suitable physical device");
        return 0;
    }
    device.createLogicalDevice();

    swapchain.createSwapchain();
    // TODO make the class have struct create info (the struct not passed on parameter)
    swapchain.createImageViews(nullptr);

    graphicPipeline.fillDefaultCreateInfo();
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
                                *device.getGraphicQueue(), vertexBuffer, vertexBufferMemory, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    indices.createVertexBuffer(*device.getPhysicalDevice(), *device.getDevice(), *commandPool.getCommandPool(),
                               *device.getGraphicQueue(), indicesBuffer, indicesBufferMemory, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);


    surface.setLoop([](){
        spdlog::debug("test");
    });
    surface.run();
}


