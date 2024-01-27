//
// Created by pach on 1/27/24.
//

#ifndef HAKUREIENGINE_IMGUIBASE_H
#define HAKUREIENGINE_IMGUIBASE_H
#include <imgui-1.90.1/imgui.h>
#include <imgui-1.90.1/backends/imgui_impl_glfw.h>
#include <imgui-1.90.1/backends/imgui_impl_vulkan.h>
#include "../HkSurface.h"
#include "../HkDevice.h"

class ImguiBase {
protected:
    HkDevice* pDevice = nullptr;
    HkSurface* pSurface = nullptr;
public:
    ImguiBase(HkDevice* pDevice, HkSurface* pSurface);

    virtual void init() = 0;
};


#endif //HAKUREIENGINE_IMGUIBASE_H
