//
// Created by pach on 1/25/24.
//
#include "util.h"

namespace util{
    void recordFrameBuffer(HkCommandPool *pCommandPool, uint32_t commandBufferIndex, VkBuffer_T *vertexBuffer,
                           VkBuffer_T *indexBuffer, uint32_t indexSize, uint32_t &imageIndex) {
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
        renderPassBeginInfo.framebuffer = (*pCommandPool->getSwapchain()->getSwapchainFramebuffers())[imageIndex];
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &pCommandPool->getGraphicPipeline()->clearColorValue;
        VkRect2D renderArea{};
        renderArea.extent = pCommandPool->getSwapchain()->getSwapExtent();
        renderArea.offset = {0, 0};
        renderPassBeginInfo.renderArea = renderArea;

        vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pCommandPool->getGraphicPipeline()->getPipeline());

        // TODO make the viewport variable inside the swapchain class
        VkViewport viewport{};
        viewport.x = 0;
        viewport.y = 0;
        VkExtent2D extent2D = pCommandPool->getSwapchain()->getSwapExtent();
        viewport.width = (float)extent2D.width;
        viewport.height = (float)extent2D.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = extent2D;

        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        VkDeviceSize offset[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, offset);
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VkIndexType::VK_INDEX_TYPE_UINT16);

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
