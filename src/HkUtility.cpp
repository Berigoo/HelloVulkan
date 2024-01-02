
#include "HkUtility.h"

bool HK::getPhysicalDeviceList(VkInstance instance, std::vector<VkPhysicalDevice> &out) {
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
    if(physicalDeviceCount == 0) return false;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, out.data());
    return true;
}

