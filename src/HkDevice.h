//
// Created by pach on 12/31/23.
//
#pragma once
#ifndef HAKUREIENGINE_HKDEVICE_H
#define HAKUREIENGINE_HKDEVICE_H
#define VK_USE_PLATFORM_XCB_KHR
#include <vulkan/vulkan.h>
#include <utility>
#include <vector>
#include <spdlog/spdlog.h>
#include <optional>
#include <set>

struct QueueFamilyIndices{
    std::optional<uint32_t> graphicFamily;
    std::optional<uint32_t> presentFamily;

    bool isCompelete(){
        return graphicFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapchainSupportDetails{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class HkDevice {
private:
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;

    QueueFamilyIndices indices;
    SwapchainSupportDetails swapchainSupport;

    std::vector<const char*> *requiredLayers = nullptr;
    std::vector<const char*> *requiredInstanceExtensions = nullptr;
    std::vector<const char*> *requiredDeviceExtensions = nullptr;

#ifdef NDEBUG
    const bool enableValidationLayer = false;
#else
    const bool enableValidationLayer = true;
#endif

public:
    HkDevice(std::vector<const char *> *requiredLayers, std::vector<const char *> *requiredExtensions);
    HkDevice(VkInstance instance);
    HkDevice();


    /// picking suitable physical device based on params
    /// \param physicalDevice1
    /// \param flags
    /// \param surface
    /// \return true if successfully pick physical device
    bool pickPhysicalDevice(VkQueueFlagBits flags, VkSurfaceKHR surface);
    void createInstance();

    void setRequiredLayers(std::vector<const char*> *layers);
    void setRequiredInstanceExtensions(std::vector<const char*> *exts);
    void setRequiredDeviceExtensions(std::vector<const char*> *exts);

    VkInstance getInstance();
    VkPhysicalDevice* getPhysicalDevice();
    std::vector<const char*>* getRequiredInstanceExtensions();
private:
    bool checkLayerSupport();
    /// check for required extensions availability on physical device extensions list
    /// \return true is available
    bool checkExtensionsSupport(VkPhysicalDevice physicalDevice1);
    /// Find the suitable queue family index based on inserted params
    /// \param physicalDevice1
    /// \param flags for checking graphic family
    /// \param surface for checking present family
    /// \return
    QueueFamilyIndices findQueueFamily(VkPhysicalDevice physicalDevice1, VkQueueFlagBits flags, VkSurfaceKHR surface);
    /// Find the swapchain details for selected surface, supported by physical device
    /// \param physicalDevice1
    /// \param surface
    /// \return
    SwapchainSupportDetails findSwapchainSupport(VkPhysicalDevice physicalDevice1, VkSurfaceKHR surface);
};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);
#endif //HAKUREIENGINE_HKDEVICE_H
