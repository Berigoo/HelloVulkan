//
// Created by pach on 12/31/23.
//
#pragma once
#ifndef HAKUREIENGINE_HKUTILITY_H
#define HAKUREIENGINE_HKUTILITY_H
#include <vector>
#include <vulkan/vulkan.h>

namespace HK{
    bool getPhysicalDeviceList(VkInstance instance, std::vector<VkPhysicalDevice>& out);
}


#endif //HAKUREIENGINE_HKUTILITY_H
