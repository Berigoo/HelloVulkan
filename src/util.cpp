//
// Created by pach on 1/25/24.
//
#include "util.h"

namespace util{
    void recordFrameBuffer(HkCommandPool *pCommandPool, uint32_t commandBufferIndex, VkBuffer *vertexBuffer,
                           VkBuffer *indexBuffer, uint32_t indexSize) {
        VkResult res;
        VkCommandBuffer commandBuffer = (*pCommandPool->getCommandBuffers())[commandBufferIndex];

        VkCommandBufferBeginInfo commandBufferBeginInfo{};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        res = vkBeginCommandBuffer((*pCommandPool->getCommandBuffers())[commandBufferIndex], &commandBufferBeginInfo);
        if(res != VK_SUCCESS) throw std::runtime_error("failed to begin command buffer");

        VkRenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = *pCommandPool->getGraphicPipeline()->getRenderPass();
        renderPassBeginInfo.framebuffer = (*pCommandPool->getSwapchain()->getSwapchainFramebuffers())[commandBufferIndex];
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &pCommandPool->getGraphicPipeline()->clearColorValue;
        renderPassBeginInfo.renderArea = pCommandPool->getGraphicPipeline()->renderArea;

        vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pCommandPool->getGraphicPipeline()->getPipeline());

        vkCmdSetViewport(commandBuffer, 0, 1, &pCommandPool->getGraphicPipeline()->viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &pCommandPool->getGraphicPipeline()->scissor);

        VkDeviceSize offset[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffer, offset);
        vkCmdBindIndexBuffer(commandBuffer, *indexBuffer, 0, VkIndexType::VK_INDEX_TYPE_UINT16);

        vkCmdDrawIndexed(commandBuffer, indexSize, 1, 0, 0, 0);

        vkCmdEndRenderPass(commandBuffer);

        res = vkEndCommandBuffer(commandBuffer);
        if(res != VK_SUCCESS) throw std::runtime_error("failed to end record command buffer");
    }

    uint32_t
    findMemoryTypeIndex(VkPhysicalDevice physicalDevice, int typeFilter, int memFlag) {
        VkPhysicalDeviceMemoryProperties memProps{};
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProps);

        for(int i=0; i<memProps.memoryTypeCount; i++){
            if((typeFilter & (1 << i)) && (memProps.memoryTypes[i].propertyFlags & memFlag) == memFlag)
                return i;
        }
    }
}
