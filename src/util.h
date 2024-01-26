//
// Created by pach on 1/25/24.
//

#ifndef HAKUREIENGINE_UTIL_H
#define HAKUREIENGINE_UTIL_H
#include <vulkan/vulkan.h>
#include "HkCommandPool.h"
#include "HkGraphicPipeline.h"
#include <glm/glm.hpp>

namespace util{
    void recordFrameBuffer(HkCommandPool *pCommandPool, uint32_t commandBufferIndex, VkBuffer *vertexBuffer,
                           VkBuffer *indexVertex, uint32_t indexSize, uint32_t &imageIndex);
    uint32_t findMemoryTypeIndex(VkPhysicalDevice physicalDevice, int typeFilter, int memFlag);
}

#endif //HAKUREIENGINE_UTIL_H
