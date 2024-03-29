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

class HkGraphicPipeline {
private:
    HkDevice* pDevice = nullptr;
public:
    HkDevice *getHKDevice() const;

private:
    HkSwapchain* pSwapchain = nullptr;
public:
    HkSwapchain *getHKSwapchain() const;

private:

    VkPipelineLayout pipelineLayout = nullptr;
    VkPipeline graphicPipeline = nullptr;
    VkRenderPass renderPass = nullptr;



public:
    std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
    };

    //VkPipeline
    std::vector<VkVertexInputAttributeDescription> attributeDescription{};
    std::vector<VkVertexInputBindingDescription> bindingDescription{};
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    VkViewport viewport{};
    VkRect2D scissor{};
    VkPipelineDynamicStateCreateInfo dynamicState{};
    VkPipelineViewportStateCreateInfo viewportState{};
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    VkPipelineMultisampleStateCreateInfo multisampling{};
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};

    //VkRenderPass
    VkAttachmentDescription colorAttachment{};
    VkAttachmentReference colorAttachmentRef{};
    VkSubpassDescription subpass{};
    VkRenderPassCreateInfo renderPassInfo{};
    VkSubpassDependency dependency{};

    VkClearValue clearColorValue{};
    VkRect2D renderArea{};


    HkGraphicPipeline(HkDevice* device, HkSwapchain* swapchain);

    /// fill all required vulkan create info struct by default value,
    /// make sure the pointer on constructor params have the required value
    void fillDefaultCreateInfo();

    void createPipelineLayout();
    void createGraphicsPipeline(VkPipelineShaderStageCreateInfo shaderStages[], uint32_t count);
    void createRenderPass();

    static const std::vector<char> readFile(const char* path);
    VkShaderModule createShaderModule(const std::vector<char>* code);

    VkPipelineLayout* getPipelineLayout();
    VkRenderPass* getRenderPass();
    VkPipeline* getPipeline();

    void cleanup();
};


#endif //HAKUREIENGINE_HKGRAPHICPIPELINE_H
