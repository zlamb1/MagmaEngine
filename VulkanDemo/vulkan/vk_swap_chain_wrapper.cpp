#include "vk_swap_chain_wrapper.h"

// Public

VkSwapChainWrapper::VkSwapChainWrapper(
    GLFWwindow& _window, 
    VkSurfaceWrapper& _vkSurfaceWrapper,
    VkDeviceWrapper& _vkDeviceWrapper) : window{ _window }, 
    vkDeviceWrapper{ _vkDeviceWrapper } {
    initSwapChain(_vkSurfaceWrapper);
    initImageViews();
}

VkSwapChainWrapper::~VkSwapChainWrapper() {
    vkDestroySwapchainKHR(vkDeviceWrapper.getLogicalDevice(), vkSwapChain, nullptr);

    for (auto imageView : swapChainImageViews) {
        vkDestroyImageView(vkDeviceWrapper.getLogicalDevice(), imageView, nullptr);
    }
}

VkFormat& VkSwapChainWrapper::getSwapChainImageFormat() {
    return vkSwapChainImageFormat;
}

VkExtent2D& VkSwapChainWrapper::getSwapChainExtent() {
    return vkSwapChainExtent;
}

std::vector<VkImageView>& VkSwapChainWrapper::getImageViews() {
    return swapChainImageViews;
}

VkSwapchainKHR& VkSwapChainWrapper::getSwapchain() {
    return vkSwapChain;
}

// Private

void VkSwapChainWrapper::initSwapChain(VkSurfaceWrapper& _vkSurfaceWrapper) {
    SwapChainSupportDetails swapChainSupport = vkDeviceWrapper.querySwapChainSupport();

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
    createInfo.surface = _vkSurfaceWrapper.getSurface();

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamily family = vkDeviceWrapper.findQueueFamily(VK_QUEUE_GRAPHICS_BIT);
    uint32_t queueFamilyIndices[] = {
        family.getGraphics(),
        family.getPresent() };

    if (family.getGraphics() != family.getPresent()) {
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

    if (vkCreateSwapchainKHR(
        vkDeviceWrapper.getLogicalDevice(), &createInfo,
        nullptr, &vkSwapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }
    else {
        vkGetSwapchainImagesKHR(vkDeviceWrapper.getLogicalDevice(),
            vkSwapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(vkDeviceWrapper.getLogicalDevice(), 
            vkSwapChain, &imageCount, swapChainImages.data());

        vkSwapChainImageFormat = surfaceFormat.format;
        vkSwapChainExtent = extent;

        std::cout << "Swap chain created!" << std::endl << std::endl;
    }
}

void VkSwapChainWrapper::initImageViews() {
    swapChainImageViews.resize(swapChainImages.size());
    for (size_t i = 0; i < swapChainImages.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];

        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = vkSwapChainImageFormat;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(vkDeviceWrapper.getLogicalDevice(),
            &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
        else {
            std::cout << "Created image view." << std::endl;
        }
    }
    std::cout << std::endl;
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
        glfwGetFramebufferSize(&window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}