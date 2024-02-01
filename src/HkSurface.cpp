//
// Created by pach on 1/6/24.
//

#include "HkSurface.h"

void HkSurface::run() {
    while(1) {
        callback();

        if(closeSignal)break;
    }
}

void HkSurface::setLoop(std::function<void()> callback) {
    this->callback = callback;
}

void HkSurface::setCloseSignal(bool val) {
    closeSignal = val;
}

void HkSurface::cleanup(VkInstance *pInstance) {
    if(surface != VK_NULL_HANDLE) vkDestroySurfaceKHR(*pInstance, surface, nullptr);
}
