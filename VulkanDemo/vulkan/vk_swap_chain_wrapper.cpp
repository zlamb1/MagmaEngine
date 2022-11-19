#include "vk_swap_chain_wrapper.h"

// Public

VkSwapChainWrapper::VkSwapChainWrapper(
    GLFWwindow& glfwWindow, 
    VkSurfaceWrapper& _vkSurfaceWrapper,
    _VkDevice& _vkDevice) : glfwWindow{ glfwWindow }, 
    _vkDevice{ _vkDevice } {

    initSwapChain(_vkSurfaceWrapper);
    initImageViews();

}

VkSwapChainWrapper::~VkSwapChainWrapper() {
    vkDestroySwapchainKHR(_vkDevice.vkDevice, vkSwapchainKHR, nullptr);

    for (auto imageView : swapchainImageViews) {
        vkDestroyImageView(_vkDevice.vkDevice, imageView, nullptr);
    }
}

VkFormat& VkSwapChainWrapper::getSwapChainImageFormat() {
    return vkSwapchainImageFormat;
}

VkExtent2D& VkSwapChainWrapper::getSwapChainExtent() {
    return vkSwapchainExtent;
}

std::vector<VkImageView>& VkSwapChainWrapper::getImageViews() {
    return swapchainImageViews;
}

VkSwapchainKHR& VkSwapChainWrapper::getSwapchain() {
    return vkSwapchainKHR;
}

// Private

void VkSwapChainWrapper::initSwapChain(VkSurfaceWrapper& _vkSurfaceWrapper) {
    auto logger = _VkLogger::Instance();

    SwapChainSupportDetails swapChainSupport = _vkDevice.querySwapChainSupport();

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0
        && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = _vkSurfaceWrapper.GetSurfaceKHR();

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    auto family = _vkDevice.vkQueueFamily;

    uint32_t queueFamilyIndices[] = {
        family.getGraphics(),
        family.getPresentation() 
    };

    if (family.getGraphics() != family.getPresentation()) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // optional
        createInfo.pQueueFamilyIndices = nullptr; // optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    auto vkCreateSwapchainResult = vkCreateSwapchainKHR(_vkDevice.vkDevice, &createInfo,
        nullptr, &vkSwapchainKHR);
    logger.LogResult("vkCreateSwapchainKHR =>", vkCreateSwapchainResult);
        
    vkGetSwapchainImagesKHR(_vkDevice.vkDevice,
        vkSwapchainKHR, &imageCount, nullptr);
        
    swapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(_vkDevice.vkDevice,
        vkSwapchainKHR, &imageCount, swapchainImages.data());

    vkSwapchainImageFormat = surfaceFormat.format;
    vkSwapchainExtent = extent;
}

void VkSwapChainWrapper::initImageViews() {
    auto logger = _VkLogger::Instance();

    swapchainImageViews.resize(swapchainImages.size());

    for (size_t i = 0; i < swapchainImages.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapchainImages[i];

        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = vkSwapchainImageFormat;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        auto vkCreateImageViewResult = vkCreateImageView(_vkDevice.vkDevice,
            &createInfo, nullptr, &swapchainImageViews[i]);
        logger.LogResult("vkCreateImageView =>", vkCreateImageViewResult);
    }
}

VkSurfaceFormatKHR VkSwapChainWrapper::chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

VkPresentModeKHR VkSwapChainWrapper::chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }
    // guaranteed to be available
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VkSwapChainWrapper::chooseSwapExtent(
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