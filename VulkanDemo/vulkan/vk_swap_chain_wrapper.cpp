#include "vk_swap_chain_wrapper.h"

namespace _VkSwapchainUtility {
    
    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }
        return availableFormats[0];
    }

    static VkPresentModeKHR chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }
        // guaranteed to be available
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    static VkExtent2D chooseSwapExtent(GLFWwindow& glfwWindow,
        const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }
        else {
            int width, height;
            glfwGetFramebufferSize(&glfwWindow, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }
}

_VkSwapchain::~_VkSwapchain() {
    if (_pDevice != nullptr) {
        vkDestroySwapchainKHR(_pDevice->vkDevice, vkSwapchainKHR, nullptr);

        for (auto vkImageView : vkImageViews) {
            vkDestroyImageView(_pDevice->vkDevice, vkImageView, nullptr);
        }
    }
}

VkResult _VkSwapchain::create() {
    auto _vkLogger = _VkLogger::Instance();

    if (pWindow == nullptr) {
        _vkLogger.LogText("_VkSwapchain => pWindow is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    if (_pDevice == nullptr) {
        _vkLogger.LogText("_VkSwapchain => _pDevice is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    if (pSurfaceKHR == nullptr) {
        _vkLogger.LogText("_VkSwapchain => pSurfaceKHR is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    auto vkSwapchainSupport = _pDevice->querySwapchainSupport();

    VkSurfaceFormatKHR surfaceFormat = _VkSwapchainUtility::chooseSwapSurfaceFormat(
        vkSwapchainSupport.formats);
    VkPresentModeKHR presentMode = _VkSwapchainUtility::chooseSwapPresentMode(
        vkSwapchainSupport.presentModes);
    VkExtent2D extent = _VkSwapchainUtility::chooseSwapExtent(*pWindow,
        vkSwapchainSupport.capabilities);

    uint32_t imageCount = vkSwapchainSupport.capabilities.minImageCount + 1;
    if (vkSwapchainSupport.capabilities.maxImageCount > 0
        && imageCount > vkSwapchainSupport.capabilities.maxImageCount) {
        imageCount = vkSwapchainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR vkCreateInfo{};
    vkCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    vkCreateInfo.surface = *pSurfaceKHR;

    vkCreateInfo.minImageCount = imageCount;
    vkCreateInfo.imageFormat = surfaceFormat.format;
    vkCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    vkCreateInfo.imageExtent = extent;
    vkCreateInfo.imageArrayLayers = 1;
    vkCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    auto vkQueueFamily = _pDevice->vkQueueFamily;

    uint32_t vkQueueIndices[] = {
        vkQueueFamily.getGraphics(),
        vkQueueFamily.getPresentation()
    };

    if (vkQueueFamily.getGraphics() != vkQueueFamily.getPresentation()) {
        vkCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        vkCreateInfo.queueFamilyIndexCount = 2;
        vkCreateInfo.pQueueFamilyIndices = vkQueueIndices;
    }
    else {
        vkCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        vkCreateInfo.queueFamilyIndexCount = 0; // optional
        vkCreateInfo.pQueueFamilyIndices = nullptr; // optional
    }

    vkCreateInfo.preTransform = vkSwapchainSupport.capabilities.currentTransform;
    vkCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    vkCreateInfo.presentMode = presentMode;
    vkCreateInfo.clipped = VK_TRUE;

    vkCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    auto vkCreateSwapchainResult = vkCreateSwapchainKHR(_pDevice->vkDevice, &vkCreateInfo,
        nullptr, &vkSwapchainKHR);
    _vkLogger.LogResult("vkCreateSwapchainKHR =>", vkCreateSwapchainResult);
        
    vkGetSwapchainImagesKHR(_pDevice->vkDevice,
        vkSwapchainKHR, &imageCount, nullptr);
        
    vkImages.resize(imageCount);
    vkGetSwapchainImagesKHR(_pDevice->vkDevice,
        vkSwapchainKHR, &imageCount, vkImages.data());

    vkSwapchainImageFormat = surfaceFormat.format;
    vkSwapchainExtent = extent;

    // Image View Init

    vkImageViews.resize(vkImages.size());

    for (size_t i = 0; i < vkImages.size(); i++) {
        VkImageViewCreateInfo vkImageViewCreateInfo{};
        vkImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        vkImageViewCreateInfo.image = vkImages[i];

        vkImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        vkImageViewCreateInfo.format = vkSwapchainImageFormat;

        vkImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        vkImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        vkImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        vkImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        vkImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        vkImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        vkImageViewCreateInfo.subresourceRange.levelCount = 1;
        vkImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        vkImageViewCreateInfo.subresourceRange.layerCount = 1;

        auto vkCreateImageViewResult = vkCreateImageView(_pDevice->vkDevice,
            &vkImageViewCreateInfo, pAllocator, &vkImageViews[i]);
        _vkLogger.LogResult("vkCreateImageView =>", vkCreateImageViewResult);
    }

    return VK_SUCCESS;
}