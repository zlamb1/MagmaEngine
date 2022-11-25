#include "vulkan_swapchain.h"

namespace swapchain_utility {
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

VulkanSwapchain::~VulkanSwapchain() {
    if (pDevice != nullptr) {
        deleteImageViews();
        vkDestroySwapchainKHR(pDevice->getDevice(), vkSwapchainKHR, nullptr);
    }
}

VkResult VulkanSwapchain::init() {
    if (pWindow == nullptr) {
        VulkanLogger::instance().enqueueText("VulkanSwapchain::init", "pWindow is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    if (pDevice == nullptr) {
        VulkanLogger::instance().enqueueText("VulkanSwapchain::init", "pDevice is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    if (pSurfaceKHR == nullptr) {
        VulkanLogger::instance().enqueueText("VulkanSwapchain::init", "pSurfaceKHR is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    auto vkSwapchainSupport = pDevice->querySwapchainSupport();

    VkSurfaceFormatKHR surfaceFormat = swapchain_utility::chooseSwapSurfaceFormat(
        vkSwapchainSupport.formats);
    VkPresentModeKHR presentMode = swapchain_utility::chooseSwapPresentMode(
        vkSwapchainSupport.presentModes);
    VkExtent2D extent = swapchain_utility::chooseSwapExtent(*pWindow,
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

    auto vkQueueFamily = pDevice->getQueueFamily();

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

    auto createSwapchainResult = vkCreateSwapchainKHR(pDevice->getDevice(), &vkCreateInfo,
        nullptr, &vkSwapchainKHR);
    VulkanLogger::instance().enqueueObject("VulkanSwapchain::init::vkCreateSwapchainResult",
        createSwapchainResult);
        
    vkGetSwapchainImagesKHR(pDevice->getDevice(), vkSwapchainKHR, &imageCount, nullptr);
        
    vkImages.resize(imageCount);
    vkGetSwapchainImagesKHR(pDevice->getDevice(), vkSwapchainKHR, &imageCount, vkImages.data());

    vkSwapchainImageFormat = surfaceFormat.format;
    vkSwapchainExtent = extent;

    // Image View Init

    vkImageViews.resize(vkImages.size());
    for (size_t i = 0; i < vkImages.size(); i++) {
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = vkImages[i];

        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = vkSwapchainImageFormat;

        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        auto createImageViewResult = vkCreateImageView(pDevice->getDevice(),
            &imageViewCreateInfo, pAllocator, &vkImageViews[i]);
        VulkanLogger::instance().enqueueObject("VulkanSwapchain::init::vkCreateImageView",
            createImageViewResult);
    }

    return VK_SUCCESS;
}

VkSwapchainKHR& VulkanSwapchain::getSwapchainKHR() {
    return vkSwapchainKHR;
}

VkFormat& VulkanSwapchain::getSwapchainImageFormat() {
    return vkSwapchainImageFormat;
}

VkExtent2D& VulkanSwapchain::getSwapchainExtent() {
    return vkSwapchainExtent;
}

std::vector<VkImage>& VulkanSwapchain::getImages() {
    return vkImages;
}

std::vector<VkImageView>& VulkanSwapchain::getImageViews() {
    return vkImageViews;
}

void VulkanSwapchain::deleteImageViews() {
    for (auto vkImageView : vkImageViews) {
        vkDestroyImageView(pDevice->getDevice(), vkImageView, nullptr);
    }
    vkImageViews.clear();
}