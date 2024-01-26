//
// Created by pach on 1/25/24.
//

#ifndef HAKUREIENGINE_VERT_H
#define HAKUREIENGINE_VERT_H
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include "util.h"

//struct vertBase{
//    virtual std::vector<VkVertexInputBindingDescription> getBinding() = 0;
//    virtual std::vector<VkVertexInputAttributeDescription> getAttribute() = 0;
//};

struct Vert2{
    glm::vec2 location;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBinding();
    static std::array<VkVertexInputAttributeDescription, 2> getAttribute();
};

template <class T>
class Vertex {
    std::vector<T> data;

public:
    Vertex(std::vector<T> vertices);

    void createVertexBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkCommandPool commandPool, VkQueue &queue,
                            VkBuffer &outVertexBuffer, VkDeviceMemory &outVertexBufferMemory, int usageFlag);
private:
    void createBuffer(VkPhysicalDevice physicalDevice, VkDevice device, int usageFlag, int memPropFlag, VkBuffer &outBuffer, VkDeviceMemory &outBufferMemory);
    void copyBuffer(VkBuffer &src, VkBuffer &dst, VkCommandPool &commandPool, VkDevice &device, VkQueue &queue);
};

template class Vertex<Vert2>;
template class Vertex<uint16_t>;
#endif //HAKUREIENGINE_VERT_H
