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

    std::function<void()> callback;

    bool closeSignal = false;

    VkSurfaceKHR surface;
public:
    bool windowResizedPoll = false;

    virtual VkSurfaceKHR* getSurface() = 0;
    virtual VkExtent2D getSurfaceExtent() = 0;
    void setLoop(std::function<void()>callback);
    void run();

    void setCloseSignal(bool val);

private:
    void cleanup(VkInstance *pInstance);
};


#endif //HAKUREIENGINE_HKSURFACE_H
