#include "vulkan_swapchain.h"

namespace SwapchainUtility {
    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && 
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
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

    static VkExtent2D chooseSwapExtent(Window::VulkanImpl& windowImpl,
        const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }
        else {
            auto framebufferSize = windowImpl.getFramebufferSize();

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(framebufferSize.first),
                static_cast<uint32_t>(framebufferSize.second)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }
}

VulkanSwapchain::VulkanSwapchain(std::shared_ptr<VulkanDevice> pVulkanDevice) : 
    pVulkanDevice{ pVulkanDevice } {}

VulkanSwapchain::~VulkanSwapchain() {
    if (pVulkanDevice != nullptr) {
        deleteImageViews();
        vkDestroySwapchainKHR(pVulkanDevice->getDevice(), vkSwapchainKHR, nullptr);
    }
}

VkResult VulkanSwapchain::init() {
    if (pVulkanDevice == nullptr) {
        Z_LOG_TXT("VulkanSwapchain::init", "pDevice is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    auto pVulkanSurface = pVulkanDevice->pVulkanSurface;
    auto vkSwapchainSupport = pVulkanDevice->querySwapchainSupport();

    VkSurfaceFormatKHR surfaceFormat = SwapchainUtility::chooseSwapSurfaceFormat(
        vkSwapchainSupport.formats);
    VkPresentModeKHR presentMode = SwapchainUtility::chooseSwapPresentMode(
        vkSwapchainSupport.presentModes);
    VkExtent2D extent = SwapchainUtility::chooseSwapExtent(pVulkanSurface->pWindowImpl,
        vkSwapchainSupport.capabilities);

    uint32_t imageCount = vkSwapchainSupport.capabilities.minImageCount + 1;
    if (vkSwapchainSupport.capabilities.maxImageCount > 0
        && imageCount > vkSwapchainSupport.capabilities.maxImageCount) {
        imageCount = vkSwapchainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR vkCreateInfo{};
    vkCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    vkCreateInfo.surface = pVulkanSurface->getSurfaceKHR();
    
    vkCreateInfo.minImageCount = imageCount;
    vkCreateInfo.imageFormat = surfaceFormat.format;
    vkCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    vkCreateInfo.imageExtent = extent;
    vkCreateInfo.imageArrayLayers = 1;
    vkCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    auto& deviceProfile = DeviceProfile::instance();
    auto graphicsQueueOpt = deviceProfile.getQueueIndices()[VulkanQueueType::GRAPHICS];

    if (!graphicsQueueOpt.has_value()) {
        Z_LOG_TXT("VulkanSwapchain::init", "could not find graphics queue");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    auto graphicsIndex = graphicsQueueOpt.value();

    uint32_t vkQueueIndices[] = {
        graphicsIndex, graphicsIndex
    };

    // TODO: get presentation queue
    if (graphicsIndex != graphicsIndex) {
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

    auto createSwapchainResult = vkCreateSwapchainKHR(pVulkanDevice->getDevice(), &vkCreateInfo,
        nullptr, &vkSwapchainKHR);
    Z_LOG_OBJ("VulkanSwapchain::init::vkCreateSwapchainResult", createSwapchainResult);
    
    if (createSwapchainResult != VK_SUCCESS) {
        return createSwapchainResult;
    }

    vkGetSwapchainImagesKHR(pVulkanDevice->getDevice(), vkSwapchainKHR, &imageCount, nullptr);
        
    vkImages.resize(imageCount);
    vkGetSwapchainImagesKHR(pVulkanDevice->getDevice(), vkSwapchainKHR, &imageCount, vkImages.data());

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

        auto createImageViewResult = vkCreateImageView(pVulkanDevice->getDevice(),
            &imageViewCreateInfo, pAllocator, &vkImageViews[i]);
        Z_LOG_OBJ("VulkanSwapchain::init::vkCreateImageView", createImageViewResult);
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
        vkDestroyImageView(pVulkanDevice->getDevice(), vkImageView, nullptr);
    }
    vkImageViews.clear();
}