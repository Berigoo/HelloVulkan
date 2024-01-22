//
// Created by pach on 1/22/24.
//

#include "HkGraphicPipeline.h"

std::vector<char> HkGraphicPipeline::readFile(const char *path) {
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if(!file.is_open()){
        throw std::runtime_error("failed to open file");
    }

    int fileSize = file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

VkShaderModule HkGraphicPipeline::createShaderModule(const std::vector<char> *code) {
    VkShaderModuleCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.codeSize = static_cast<uint32_t>(code->size());
    info.pCode = reinterpret_cast<const uint32_t*>(code);

    VkShaderModule shaderModule;
    if(vkCreateShaderModule(*pDevice->getDevice(), &info, nullptr, &shaderModule) != VK_SUCCESS){
        throw std::runtime_error("failed to create shader module");
    }

    return shaderModule;
}

HkGraphicPipeline::HkGraphicPipeline(HkDevice *device) {
    pDevice = device;
    //vert
    attributeDescription[0].binding = 0;
    attributeDescription[0].location = 0;
    attributeDescription[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescription[0].offset = offsetof(Vertex, pos);
    //color
    attributeDescription[1].binding = 0;
    attributeDescription[1].location = 1;
    attributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescription[1].offset = offsetof(Vertex, color);

    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();

    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkExtent2D extent2D = pSwapchain->getSwapExtent();
    viewport.x = 0.0;
    viewport.y = 0.0;
    viewport.width = (float) extent2D.width;
    viewport.height = (float) extent2D.height;
    viewport.minDepth = 0.0;
    viewport.maxDepth = 1.0f;

    scissor.offset = {0, 0};
    scissor.extent = extent2D;
}
