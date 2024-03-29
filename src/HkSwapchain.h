//
// Created by pach on 1/6/24.
//

#ifndef HAKUREIENGINE_HKSWAPCHAIN_H
#define HAKUREIENGINE_HKSWAPCHAIN_H
#include "HkDevice.h"
#include "HkSurface.h"
#include "HkSyncObject.h"
class HkGraphicPipeline;

class HkSwapchain {
private:
    HkDevice* pDevice = nullptr;
    HkSurface* pSurface = nullptr;
    HkSyncObject* pSyncObject = nullptr;

    SwapchainSupportDetails swapchainSupportInfo;
    VkSwapchainKHR swapchain = nullptr;
    VkSwapchainKHR oldSwapchain = nullptr;

    VkFormat format = VK_FORMAT_B8G8R8A8_SRGB;
    VkColorSpaceKHR colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;
    std::vector<VkFramebuffer> swapchainFramebuffers;
public:
    HkSwapchain(HkDevice *device, HkSurface *surface, HkSyncObject *syncObject);

    VkImageViewCreateInfo imageViewCreateInfo{};

    void setFormat(VkFormat format1);
    void setColorSpace(VkColorSpaceKHR colorSpace1);
    void setPresentMode(VkPresentModeKHR presentMode1);

    std::vector<VkImageView>* getSwapchainImageViews();
    std::vector<VkImage>* getSwapchainImages();
    std::vector<VkFramebuffer>* getSwapchainFramebuffers();
    HkSyncObject* getSyncObject();
    VkSwapchainKHR* getSwapchain();

    VkFormat getFormat();

    /// creating swapchain based on class variables
    void createSwapchain();
    void createImageViews();
    void createFramebuffers(HkGraphicPipeline* pGraphicPipeline);
    /// if the current extent shows valid value then return it, otherwise creating extent
    /// from surface geometry commandPoolInfo
    /// \return
    VkExtent2D getSwapExtent();
    void recreateSwapchain(HkGraphicPipeline *pGraphicPipeline);
    /// fill swapChainSupportInfo variable,
    /// to find supported commandPoolInfo by swapchain before creating it
    void findSwapchainSupport();
    /// choosing the surface format based on params from the available supported formats
    /// \return
    VkSurfaceFormatKHR chooseSwapSurfaceFormat();
    /// choosing the surface present mode based on params  from the available supported present modes
    /// \param mode
    /// \return default VK_PRESENT_MODE_FIFO_KHR
    VkPresentModeKHR chooseSwapSurfacePresentMode();

    void cleanup();
};


#endif //HAKUREIENGINE_HKSWAPCHAIN_H
