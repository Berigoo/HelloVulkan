//
// Created by pach on 1/22/24.
//

#ifndef HAKUREIENGINE_HKGRAPHICPIPELINE_H
#define HAKUREIENGINE_HKGRAPHICPIPELINE_H
#include <vulkan/vulkan.h>
#include <vector>
#include <fstream>
#include "HkDevice.h"
#include "HkSwapchain.h"
#include <glm/glm.hpp>

struct Vertex{
    glm::vec2 pos;
    glm::vec3 color;
};

class HkGraphicPipeline {
private:
    HkDevice* pDevice;
    HkSwapchain* pSwapchain;

public:
    std::array<VkVertexInputAttributeDescription, 2> attributeDescription{};
    VkVertexInputBindingDescription bindingDescription{};
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    VkViewport viewport{};
    VkRect2D scissor{};


    HkGraphicPipeline(HkDevice* device);

    static std::vector<char> readFile(const char* path);

private:
    VkShaderModule createShaderModule(const std::vector<char>* code);
};


#endif //HAKUREIENGINE_HKGRAPHICPIPELINE_H
