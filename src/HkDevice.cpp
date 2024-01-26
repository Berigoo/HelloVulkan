//
// Created by pach on 12/31/23.
//

#include "HkDevice.h"

HkDevice::HkDevice(std::vector<const char *> *requiredLayers, std::vector<const char *> *requiredExtensions) {
//    this->requiredLayers = requiredLayers;
//    this->requiredInstanceExtensions = requiredExtensions;
//    // creating instance
//    VkApplicationInfo appInfo{};
//    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
//    appInfo.pApplicationName = "Basic App";
//    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
//    appInfo.pEngineName = "Hakurei Engine";
//    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
//    appInfo.apiVersion = VK_API_VERSION_1_0;
//
//    VkInstanceCreateInfo instanceCreateInfo{};
//    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
//    instanceCreateInfo.enabledLayerCount = 0;
//    if(enableValidationLayer && checkLayerSupport()){
//        spdlog::set_level(spdlog::level::trace);
//        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t >(requiredLayers->size());
//        instanceCreateInfo.ppEnabledLayerNames = requiredLayers->data();
//
//        // debug object
//        VkDebugUtilsMessengerCreateInfoEXT debugInfo{};
//        debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
//        debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
//        debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
//        debugInfo.pfnUserCallback = debugCallback;
//
//        instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugInfo;
//    }
//    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t >(requiredExtensions->size());
//    instanceCreateInfo.ppEnabledExtensionNames = requiredExtensions->data();
//
//    if(vkCreateInstance(&instanceCreateInfo, nullptr, instance) != VK_SUCCESS){
//        throw std::runtime_error("failed to create vulkan instance");
//    }
}

bool HkDevice::checkLayerSupport() {
    uint32_t propCount = 0;
    vkEnumerateInstanceLayerProperties(&propCount, nullptr);
    if (propCount == 0) return false;
    std::vector<VkLayerProperties> propLayers;
    vkEnumerateInstanceLayerProperties(&propCount, propLayers.data());

    for (auto reqLayer: *requiredLayers) {
        for (uint32_t i = 0; i < propLayers.size(); i++) {
            if (reqLayer == propLayers[i].layerName) break;
            else if (i == propLayers.size() - 1) {
                spdlog::error("Required layer not available!");
                return false;
            }
        }
    }
    return true;
}

HkDevice::HkDevice(VkInstance instance) {

}

bool HkDevice::pickPhysicalDevice(VkQueueFlagBits flags, VkSurfaceKHR surface) {
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
    if (physicalDeviceCount == 0) throw std::runtime_error("Physical device not found");
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

    for (auto phyDev: physicalDevices) {
        indices = findQueueFamily(phyDev, flags, surface);
        bool isExtsSupported = checkExtensionsSupport(phyDev);
        bool isSwapchainSupported = false;

        // if the VK_KHR_SWAPCHAIN_EXTENSION_NAME supported then swapchain supported for this surface
        if (isExtsSupported) {
            swapchainSupport = findSwapchainSupport(phyDev, surface);
            isSwapchainSupported = !swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
        }

        if (indices.isCompelete() && isExtsSupported && isSwapchainSupported) {
            physicalDevice = phyDev;
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(phyDev, &props);
            spdlog::debug("picked physical device name : {}", props.deviceName);
            return true;
        }
    }
    return false;
}

QueueFamilyIndices
HkDevice::findQueueFamily(VkPhysicalDevice physicalDevice1, VkQueueFlagBits flags, VkSurfaceKHR surface) {
    QueueFamilyIndices holder;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice1, &queueFamilyCount, nullptr);
    if (queueFamilyCount == 0) throw std::runtime_error("Queue family count 0");
    std::vector<VkQueueFamilyProperties> availableProps(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice1, &queueFamilyCount, availableProps.data());

    int i = 0;
    for (const auto &prop: availableProps) {
        VkBool32 presentSupported = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice1, i, surface, &presentSupported);
        if (prop.queueFlags & flags) {
            holder.graphicFamily = i;
        }
        if (presentSupported) {
            holder.presentFamily = i;
        }
        if (holder.isCompelete()) break;
        i++;
    }

    return holder;
}

bool HkDevice::checkExtensionsSupport(VkPhysicalDevice physicalDevice1) {
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(physicalDevice1, nullptr, &extensionCount, nullptr);
    if (extensionCount == 0) throw std::runtime_error("Extension count 0");
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice1, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions1(requiredDeviceExtensions->begin(), requiredDeviceExtensions->end());

    spdlog::trace("physical device exts support");
    for (auto ext: availableExtensions) {
        spdlog::trace("\t{}", ext.extensionName);
        requiredExtensions1.erase(ext.extensionName);
    }

    return requiredExtensions1.empty();
}

SwapchainSupportDetails HkDevice::findSwapchainSupport(VkPhysicalDevice physicalDevice1, VkSurfaceKHR surface) {
    SwapchainSupportDetails holder;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice1, surface, &holder.capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice1, surface, &formatCount, nullptr);
    if (formatCount == 0) throw std::runtime_error("surface format 0 supported");
    holder.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice1, surface, &formatCount, holder.formats.data());

    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice1, surface, &presentModeCount, nullptr);
    if (presentModeCount == 0) throw std::runtime_error("present mode 0 supported");
    holder.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice1, surface, &presentModeCount, holder.presentModes.data());

    return holder;
}

VkPhysicalDevice *HkDevice::getPhysicalDevice() {
    if (physicalDevice) return &physicalDevice;
    else {
        spdlog::warn("physical device null");
        return nullptr;
    }
}

HkDevice::HkDevice() {

}

void HkDevice::setRequiredLayers(std::vector<const char *> *layers) {
    if (!layers) spdlog::warn("layers null");
    this->requiredLayers = layers;
}

void HkDevice::setRequiredDeviceExtensions(std::vector<const char *> *exts) {
    if (!exts) spdlog::warn("device extensions null");
    this->requiredDeviceExtensions = exts;
}

void HkDevice::setRequiredInstanceExtensions(std::vector<const char *> *exts) {
    if (!exts) spdlog::warn("instance extensions null");
    requiredInstanceExtensions = exts;
}

std::vector<const char *> *HkDevice::getRequiredInstanceExtensions() {
    return requiredInstanceExtensions;
}

void HkDevice::createInstance() {
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
    instanceCreateInfo.pApplicationInfo = &appInfo;
    if (enableValidationLayer && checkLayerSupport()) {
        spdlog::set_level(spdlog::level::trace);
        if (requiredLayers) {
            instanceCreateInfo.enabledLayerCount = static_cast<uint32_t >(requiredLayers->size());
            instanceCreateInfo.ppEnabledLayerNames = requiredLayers->data();
        } else {
            spdlog::warn("required layers null");
        }

        // debug object
        VkDebugUtilsMessengerCreateInfoEXT debugInfo{};
        debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugInfo.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
        debugInfo.messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugInfo.pfnUserCallback = debugCallback;

        instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugInfo;
    }
    if (requiredInstanceExtensions) {
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t >(requiredInstanceExtensions->size());
        instanceCreateInfo.ppEnabledExtensionNames = requiredInstanceExtensions->data();
    } else {
        spdlog::warn("required instance extensions null");
    }

    if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vulkan instance");
    }
}

VkInstance HkDevice::getInstance() {
    return instance;
}

void HkDevice::createLogicalDevice() {
    std::vector<VkDeviceQueueCreateInfo> queuesCreateInfo;
    std::set<uint32_t> uniqueQueueFamily = {indices.graphicFamily.value(), indices.presentFamily.value()};
    float queuePriority = 1.0f;
    for (auto queueFamily: uniqueQueueFamily) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queuesCreateInfo.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures physicalDeviceFeatures{};

    VkDeviceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.queueCreateInfoCount = static_cast<uint32_t>(queuesCreateInfo.size());
    info.pQueueCreateInfos = queuesCreateInfo.data();
    info.pEnabledFeatures = &physicalDeviceFeatures;
    info.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions->size());
    info.ppEnabledExtensionNames = requiredDeviceExtensions->data();
    if (enableValidationLayer && checkLayerSupport()) {
        info.enabledLayerCount = static_cast<uint32_t>(requiredLayers->size());
        info.ppEnabledLayerNames = requiredLayers->data();
    } else {
        info.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice, &info, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device");
    }

    vkGetDeviceQueue(device, indices.graphicFamily.value(), 0, &graphicQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

void HkDevice::createSwapchain() {

}

QueueFamilyIndices *HkDevice::getQueueFamilyIndices() {
    return &indices;
}

VkDevice *HkDevice::getDevice() {
    return &device;
}

VkQueue *HkDevice::getGraphicQueue() {
    return &graphicQueue;
}

VkQueue *HkDevice::getPresentQueue() {
    return &presentQueue;
}

VkBool32
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
    spdlog::debug("msg : {}", pCallbackData->pMessage);
    return VK_FALSE;
}
