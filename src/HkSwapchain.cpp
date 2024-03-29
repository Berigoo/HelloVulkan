//
// Created by pach on 1/6/24.
//

#include "HkSwapchain.h"
#include "HkGraphicPipeline.h"

HkSwapchain::HkSwapchain(HkDevice *device, HkSurface *surface, HkSyncObject *syncObject) {
    if(!device || !surface){
        spdlog::error("device or surface null");
        return;
    }

    pDevice = device;
    pSurface = surface;
    pSyncObject = syncObject;


    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = format;
    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;
}

void HkSwapchain::findSwapchainSupport() {
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*pDevice->getPhysicalDevice(), *pSurface->getSurface(), &swapchainSupportInfo.capabilities);

    uint32_t formatsCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(*pDevice->getPhysicalDevice(), *pSurface->getSurface(), &formatsCount, nullptr);
    if(formatsCount == 0){
        throw std::runtime_error("surface supported format 0");
    }
    swapchainSupportInfo.formats.resize(formatsCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(*pDevice->getPhysicalDevice(), *pSurface->getSurface(), &formatsCount, swapchainSupportInfo.formats.data());

    uint32_t presentModesCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(*pDevice->getPhysicalDevice(), *pSurface->getSurface(), &presentModesCount, nullptr);
    if(presentModesCount == 0){
        throw std::runtime_error("surface supported present mode 0");
    }
    swapchainSupportInfo.presentModes.resize(presentModesCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(*pDevice->getPhysicalDevice(), *pSurface->getSurface(), &presentModesCount, swapchainSupportInfo.presentModes.data());
}

VkSurfaceFormatKHR HkSwapchain::chooseSwapSurfaceFormat() {
    for(const auto& availableFormat : swapchainSupportInfo.formats){
        if(availableFormat.format == format && availableFormat.colorSpace == colorSpace)
            return availableFormat;
    }
    spdlog::warn("requested format not available");
    return swapchainSupportInfo.formats[0];
}

VkPresentModeKHR HkSwapchain::chooseSwapSurfacePresentMode() {
    for(const auto& availableMode : swapchainSupportInfo.presentModes ){
        if(availableMode == presentMode){
            return availableMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D HkSwapchain::getSwapExtent() {
    if(swapchainSupportInfo.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()){
        return swapchainSupportInfo.capabilities.currentExtent;
    }else{
        VkExtent2D extent = pSurface->getSurfaceExtent();
        std::clamp(extent.width, swapchainSupportInfo.capabilities.minImageExtent.width, swapchainSupportInfo.capabilities.maxImageExtent.width);
        std::clamp(extent.height, swapchainSupportInfo.capabilities.minImageExtent.height, swapchainSupportInfo.capabilities.maxImageExtent.height);
        return extent;
    }
}

void HkSwapchain::createSwapchain() {
    findSwapchainSupport();

    VkSwapchainCreateInfoKHR info{};
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.surface = *pSurface->getSurface();
    uint32_t imageCount = swapchainSupportInfo.capabilities.minImageCount + 1;
    if(swapchainSupportInfo.capabilities.maxImageCount > 0 && imageCount > swapchainSupportInfo.capabilities.maxImageCount){
        info.minImageCount = swapchainSupportInfo.capabilities.maxImageCount;
    }else{
        info.minImageCount = swapchainSupportInfo.capabilities.minImageCount + 1;
    }
    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat();
    info.imageFormat = surfaceFormat.format;
    info.imageColorSpace = surfaceFormat.colorSpace;
    info.imageExtent = getSwapExtent();
    info.presentMode = chooseSwapSurfacePresentMode();
    // TODO make it editable
    info.imageArrayLayers = 1;
    info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices* index = pDevice->getQueueFamilyIndices();
    uint32_t queueFamilyIndices[] = {index->presentFamily.value(), index->graphicFamily.value()};
    if(index->graphicFamily != index->presentFamily){
        info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        info.queueFamilyIndexCount = 2;
        info.pQueueFamilyIndices = queueFamilyIndices;
    }else{
        info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        info.queueFamilyIndexCount = 0;
        info.pQueueFamilyIndices = nullptr;
    }

    info.preTransform = swapchainSupportInfo.capabilities.currentTransform;
    info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    info.clipped = VK_TRUE;
    if(oldSwapchain != VK_NULL_HANDLE) info.oldSwapchain = oldSwapchain;
    else info.oldSwapchain = VK_NULL_HANDLE;

    if(vkCreateSwapchainKHR(*pDevice->getDevice(), &info, nullptr, &swapchain) != VK_SUCCESS){
        throw std::runtime_error("failed to create swapchain");
    }

    uint32_t imagesCount = 0;
    vkGetSwapchainImagesKHR(*pDevice->getDevice(), swapchain, &imagesCount, nullptr);
    swapchainImages.resize(imagesCount);
    vkGetSwapchainImagesKHR(*pDevice->getDevice(), swapchain, &imagesCount, swapchainImages.data());
}

void HkSwapchain::setFormat(VkFormat format1) {
    format = format1;
}

void HkSwapchain::setColorSpace(VkColorSpaceKHR colorSpace1) {
    colorSpace = colorSpace1;
}

void HkSwapchain::setPresentMode(VkPresentModeKHR presentMode1) {
    presentMode = presentMode1;
}

void HkSwapchain::createImageViews() {
    swapchainImageViews.resize(swapchainImages.size());
    for (int i = 0; i < swapchainImages.size(); i++) {

        imageViewCreateInfo.image = swapchainImages[i];

        if (vkCreateImageView(*pDevice->getDevice(), &imageViewCreateInfo, nullptr, &swapchainImageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views");
        }
    }
}

VkFormat HkSwapchain::getFormat() {
    return format;
}

std::vector<VkImageView> *HkSwapchain::getSwapchainImageViews() {
    if(swapchainImageViews.size() < 1){
        spdlog::error("swapchain image view size lower than 1");
    }
    return &swapchainImageViews;
}

void HkSwapchain::createFramebuffers(HkGraphicPipeline *pGraphicPipeline) {
    swapchainFramebuffers.resize(swapchainImageViews.size());

    for(int i=0; i<swapchainImageViews.size(); i++) {
        VkImageView attachments[] = {
                swapchainImageViews[i]
        };

        VkFramebufferCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        if(pGraphicPipeline->getRenderPass()) {
            createInfo.renderPass = *pGraphicPipeline->getRenderPass();
        }else{
            throw std::runtime_error("failed to create framebuffers, render pass null");
        }
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = attachments;
        VkExtent2D extent2D = getSwapExtent();
        createInfo.width = extent2D.width;
        createInfo.height = extent2D.height;
        createInfo.layers = 1;

        if (vkCreateFramebuffer(*pDevice->getDevice(), &createInfo, nullptr, &swapchainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffers");
        }
    }
}

std::vector<VkImage> *HkSwapchain::getSwapchainImages() {
    return &swapchainImages;
}

std::vector<VkFramebuffer> *HkSwapchain::getSwapchainFramebuffers() {
    return &swapchainFramebuffers;
}

HkSyncObject *HkSwapchain::getSyncObject() {
    return pSyncObject;
}

VkSwapchainKHR *HkSwapchain::getSwapchain() {
    return &swapchain;
}

void HkSwapchain::cleanup() {
    for(int i=0; i<swapchainImages.size(); i++) {
        vkDestroyImageView(*pDevice->getDevice(), swapchainImageViews[i], nullptr);
        vkDestroyFramebuffer(*pDevice->getDevice(), swapchainFramebuffers[i], nullptr);
    }
    if(oldSwapchain != VK_NULL_HANDLE) vkDestroySwapchainKHR(*pDevice->getDevice(), oldSwapchain, nullptr);
    vkDestroySwapchainKHR(*pDevice->getDevice(), swapchain, nullptr);
}

void HkSwapchain::recreateSwapchain(HkGraphicPipeline *pGraphicPipeline) {
    vkDeviceWaitIdle(*pDevice->getDevice());

    for(int i=0; i<swapchainImages.size(); i++) {
        vkDestroyImageView(*pDevice->getDevice(), swapchainImageViews[i], nullptr);
        vkDestroyFramebuffer(*pDevice->getDevice(), swapchainFramebuffers[i], nullptr);
    }
    if(oldSwapchain != VK_NULL_HANDLE) vkDestroySwapchainKHR(*pDevice->getDevice(), oldSwapchain, nullptr);

    oldSwapchain = swapchain;

    createSwapchain();
    createImageViews();
    createFramebuffers(pGraphicPipeline);
}


