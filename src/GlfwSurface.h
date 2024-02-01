//
// Created by pach on 1/27/24.
//

#ifndef HAKUREIENGINE_GLFWSURFACE_H
#define HAKUREIENGINE_GLFWSURFACE_H
#include "HkSurface.h"
#include "HkDevice.h"
#include <GLFW/glfw3.h>

class GlfwSurface : public HkSurface{
private:
    GLFWwindow* window = nullptr;
    HkDevice* pDevice = nullptr;
public:
    GlfwSurface(HkDevice *pDevice);

    ~GlfwSurface()= default;

    void createSurface(int width, int height, const char* title);

    VkSurfaceKHR * getSurface() override;

    VkExtent2D getSurfaceExtent() override;

    GLFWwindow* getWindow();

    void cleanup(VkInstance* pInstance) override;

private:

};


#endif //HAKUREIENGINE_GLFWSURFACE_H
