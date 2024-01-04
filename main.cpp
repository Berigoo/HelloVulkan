#include <iostream>
#include "src/Hk.h"

// TODO do some decision based on used window system manager name
//  not hard coded like this
//  maybe, every surface class must have its own must-needed extensions name
//  then the value pushed-back into device class required-extensions
std::vector<const char*> requiredLayers1 = {"VK_LAYER_KHRONOS_validation"};
std::vector<const char*> requiredExtensionsXcb = {VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_XCB_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
std::vector<const char*> requiredDeviceExtensions1 = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

int main() {
    HkDevice device;
    device.setRequiredLayers(&requiredLayers1);
    device.setRequiredInstanceExtensions(&requiredExtensionsXcb);
    device.setRequiredDeviceExtensions(&requiredDeviceExtensions1);

    device.createInstance();
    XcbSurface surface(500, 800, (XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK), (XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY));

    surface.createSurface(&device);

    if(!device.pickPhysicalDevice(VK_QUEUE_GRAPHICS_BIT, *surface.getSurface())){
        spdlog::error("failed picking suitable physical device");
        return 0;
    }

}
