//
// Created by pach on 1/27/24.
//

#include "GlfwSurface.h"

VkExtent2D GlfwSurface::getSurfaceExtent() {
    return VkExtent2D();
}

GlfwSurface::GlfwSurface(HkDevice *pDevice) {
    this->pDevice = pDevice;
}

GLFWwindow *GlfwSurface::getWindow() {
    return window;
}

VkSurfaceKHR *GlfwSurface::getSurface() {
    return &surface;
}

void GlfwSurface::init(int width, int height, const char *title) {
    if(!glfwInit()) throw std::runtime_error("cannot init glfw");
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if(!glfwVulkanSupported()) throw std::runtime_error("vulkan not supported");
    VkResult res;
    if((res = glfwCreateWindowSurface(pDevice->getInstance(), window, nullptr, &surface) )!= VK_SUCCESS){
        spdlog::error("res, {}", res);
        throw std::runtime_error("failed to create surface");
    }
}
