#include "swapchain.h"

namespace Magma {

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

        static VkExtent2D chooseSwapExtent(const VulkanImpl& windowImpl,
            const VkSurfaceCapabilitiesKHR& capabilities) {
            if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
                return capabilities.currentExtent;
            }
            else {
                auto [width, height] = windowImpl.getFramebufferSize();

                VkExtent2D actualExtent = {
                    static_cast<uint32_t>(width), static_cast<uint32_t>(height)
                };

                actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
                actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

                return actualExtent;
            }
        }
    }

    Swapchain::Swapchain(std::shared_ptr<VulkanDevice> device) :
        m_Device{ std::move(device)} {}

    Swapchain::~Swapchain() {
        if (m_Device != nullptr) {
            deleteImageViews();
            vkDestroySwapchainKHR(m_Device->getDevice(), m_SwapchainKHR, nullptr);
        }
    }

    VkResult Swapchain::init() {
        if (m_Device == nullptr) {
            Z_LOG_TXT("VulkanSwapchain::init", "pDevice is nullptr");
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        const auto pVulkanSurface = m_Device->pVulkanSurface;
        auto vkSwapchainSupport = m_Device->querySwapchainSupport();

        VkSurfaceFormatKHR surfaceFormat = SwapchainUtility::chooseSwapSurfaceFormat(
            vkSwapchainSupport.formats);
        VkPresentModeKHR presentMode = SwapchainUtility::chooseSwapPresentMode(
            vkSwapchainSupport.presentModes);
        VkExtent2D extent = SwapchainUtility::chooseSwapExtent(pVulkanSurface->m_WindowImpl,
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

        auto createSwapchainResult = vkCreateSwapchainKHR(m_Device->getDevice(), &vkCreateInfo,
            nullptr, &m_SwapchainKHR);
        Z_LOG_OBJ("VulkanSwapchain::init::vkCreateSwapchainResult", createSwapchainResult);

        if (createSwapchainResult != VK_SUCCESS) {
            return createSwapchainResult;
        }

        vkGetSwapchainImagesKHR(m_Device->getDevice(), m_SwapchainKHR, &imageCount, nullptr);

        m_Images.resize(imageCount);
        vkGetSwapchainImagesKHR(m_Device->getDevice(), m_SwapchainKHR, &imageCount, m_Images.data());

        m_SwapchainImageFormat = surfaceFormat.format;
        m_SwapchainExtent = extent;

        // Image View Init

        m_ImageViews.resize(m_Images.size());
        for (size_t i = 0; i < m_Images.size(); i++) {
            VkImageViewCreateInfo imageViewCreateInfo{};
            imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewCreateInfo.image = m_Images[i];

            imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imageViewCreateInfo.format = m_SwapchainImageFormat;

            imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
            imageViewCreateInfo.subresourceRange.levelCount = 1;
            imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            imageViewCreateInfo.subresourceRange.layerCount = 1;

            auto createImageViewResult = vkCreateImageView(m_Device->getDevice(),
                &imageViewCreateInfo, pAllocator, &m_ImageViews[i]);
            Z_LOG_OBJ("VulkanSwapchain::init::vkCreateImageView", createImageViewResult);
        }

        return VK_SUCCESS;
    }

    VkSwapchainKHR& Swapchain::getSwapchainKHR() {
        return m_SwapchainKHR;
    }

    VkFormat& Swapchain::getSwapchainImageFormat() {
        return m_SwapchainImageFormat;
    }

    VkExtent2D& Swapchain::getSwapchainExtent() {
        return m_SwapchainExtent;
    }

    std::vector<VkImage>& Swapchain::getImages() {
        return m_Images;
    }

    std::vector<VkImageView>& Swapchain::getImageViews() {
        return m_ImageViews;
    }

    void Swapchain::deleteImageViews() {
        for (const auto imageView : m_ImageViews) {
            vkDestroyImageView(m_Device->getDevice(), imageView, pAllocator);
        }
        m_ImageViews.clear();
    }

}