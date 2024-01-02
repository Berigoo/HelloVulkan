//
// Created by pach on 12/31/23.
//

#include "HkDevice.h"

HkDevice::HkDevice() {
    // creating instance
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Basic App";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Hakurei Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.enabledLayerCount = 0;
    if(enableValidationLayer && checkLayerSupport()){
        instanceCreateInfo.enabledLayerCount = requiredLayers.size();
        instanceCreateInfo.ppEnabledLayerNames = requiredLayers.data();

        // debug object
        VkDebugUtilsMessengerCreateInfoEXT debugInfo{};
        debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
        debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugInfo.pfnUserCallback = debugCallback;

        instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugInfo;
    }
    vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
}

bool HkDevice::checkLayerSupport() {
    uint32_t propCount = 0;
    vkEnumerateInstanceLayerProperties(&propCount, nullptr);
    if(propCount == 0) return false;
    std::vector<VkLayerProperties> propLayers;
    vkEnumerateInstanceLayerProperties(&propCount, propLayers.data());

    for(auto reqLayer : requiredLayers){
        for(uint32_t i=0; i<propLayers.size(); i++){
            if(reqLayer == propLayers[i].layerName) break;
            else if(i == propLayers.size()-1){
                spdlog::error("Required layer not available!");
                return false;
            }
        }
    }
    return true;
}

HkDevice::HkDevice(VkInstance instance) {

}

void HkDevice::setRequiredLayers(std::vector<const char *> reqLayers) {
    requiredLayers = std::move(reqLayers);
}

bool HkDevice::pickPhysicalDevice(VkQueueFlagBits flags, VkSurfaceKHR surface) {
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
    if(physicalDeviceCount == 0) throw std::runtime_error("Physical device not found");
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

    for(auto phyDev : physicalDevices){
        indices = findQueueFamily(phyDev, flags, surface);
        bool isExtsSupported = checkExtensionsSupport(phyDev);
        bool isSwapchainSupported = false;

        // if the VK_KHR_SWAPCHAIN_EXTENSION_NAME supported then swapchain supported for this surface
        if(isExtsSupported){
            swapchainSupport = findSwapchainSupport(phyDev, surface);
            isSwapchainSupported = !swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
        }

        if(indices.isCompelete() && isExtsSupported && isSwapchainSupported) return true;
    }
    return false;
}

QueueFamilyIndices
HkDevice::findQueueFamily(VkPhysicalDevice physicalDevice1, VkQueueFlagBits flags, VkSurfaceKHR surface) {
    QueueFamilyIndices holder;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice1, &queueFamilyCount, nullptr);
    if(queueFamilyCount == 0) throw std::runtime_error("Queue family count 0");
    std::vector<VkQueueFamilyProperties> availableProps(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice1, &queueFamilyCount, availableProps.data());

    uint32_t i = 0;
    for(auto prop : availableProps){
        if(prop.queueFlags & flags){
            holder.graphicFamily = i;
            VkBool32 presentSupported = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice1, i, surface, &presentSupported);
            if(presentSupported){
                holder.presentFamily = i;
                break;
            }
        }
        i++;
    }

    return holder;
}

bool HkDevice::checkExtensionsSupport(VkPhysicalDevice physicalDevice1) {
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(physicalDevice1, nullptr, &extensionCount, nullptr);
    if(extensionCount == 0) throw std::runtime_error("Extension count 0");
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice1, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions1(requiredExtensions.begin(), requiredExtensions.end());

    for(auto ext : availableExtensions){
        requiredExtensions1.erase(ext.extensionName);
    }

    return requiredExtensions1.empty();
}

void HkDevice::setRequiredExtensions(std::vector<const char *> reqExtensions) {
    requiredExtensions = std::move(reqExtensions);
}

SwapchainSupportDetails HkDevice::findSwapchainSupport(VkPhysicalDevice physicalDevice1, VkSurfaceKHR surface) {
    SwapchainSupportDetails holder;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice1, surface, &holder.capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice1, surface, &formatCount, nullptr);
    if(formatCount == 0) throw std::runtime_error("surface format 0 supported");
    holder.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice1, surface, &formatCount, holder.formats.data());

    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice1, surface, &presentModeCount, nullptr);
    if(presentModeCount == 0) throw std::runtime_error("present mode 0 supported");
    holder.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice1, surface, &presentModeCount, holder.presentModes.data());

    return holder;
}

VkBool32
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
   spdlog::debug(pCallbackData->pMessage);
}
