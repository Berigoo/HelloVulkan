//
// Created by pach on 1/6/24.
//

#ifndef HAKUREIENGINE_HKSURFACE_H
#define HAKUREIENGINE_HKSURFACE_H


#include <vulkan/vulkan.h>
#include <spdlog/spdlog.h>

class HkSurface {
protected:
    HkSurface(){};
    ~HkSurface()= default;
public:
    virtual VkSurfaceKHR* getSurface() = 0;
    virtual VkExtent2D getSurfaceExtent() = 0;
};


#endif //HAKUREIENGINE_HKSURFACE_H
