#include "vulkan_sync.h";

// VulkanSemaphore

VulkanSemaphore::~VulkanSemaphore() {
    if (pDevice != nullptr) {
        vkDestroySemaphore(*pDevice, vkSemaphore, nullptr);
    }
}

VkResult VulkanSemaphore::init() {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (pDevice == nullptr) {
        Z_LOG_TXT("VulkanSemaphore::init", "pDevice is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    return vkCreateSemaphore(*pDevice, &semaphoreInfo, nullptr, &vkSemaphore);
}

VkSemaphore& VulkanSemaphore::getSemaphore() {
    return vkSemaphore;
}

// VulkanFence

VulkanFence::~VulkanFence() {
    if (pDevice != nullptr) {
        vkDestroyFence(*pDevice, vkFence, pAllocator);
    }
}

VkResult VulkanFence::init() {    
    if (pDevice == nullptr) {
        Z_LOG_TXT("VulkanFence::init", "pDevice is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    return vkCreateFence(*pDevice, &fenceInfo, pAllocator, &vkFence);
}

VkFence& VulkanFence::getFence() {
    return vkFence;
}

// VulkanRenderSync

VkResult VulkanRenderSync::init() {
    if (pDevice == nullptr) {
        Z_LOG_TXT("VulkanRenderSync::init", "pDevice is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    flightFence.pDevice = pDevice;

    auto flightFenceResult = flightFence.init();
    Z_LOG_OBJ("VulkanRenderSync::init::flightFence", flightFenceResult);

    if (flightFenceResult != VK_SUCCESS) {
        return flightFenceResult;
    }

    imageSemaphore.pDevice = pDevice;
    auto imageSemaphoreResult = imageSemaphore.init();
    Z_LOG_OBJ("VulkanRenderSync::init::imageSemaphore", imageSemaphoreResult);
    if (imageSemaphoreResult != VK_SUCCESS) {
        return imageSemaphoreResult;
    }

    renderSemaphore.pDevice = pDevice;
    auto renderSemaphoreResult = renderSemaphore.init();
    Z_LOG_OBJ("VulkanRenderSync::init::renderSemaphore", renderSemaphoreResult);
    if (renderSemaphoreResult != VK_SUCCESS) {
        return renderSemaphoreResult;
    }

    return VK_SUCCESS;
}

VulkanFence& VulkanRenderSync::getFlightFence() {
    return flightFence;
}

VulkanSemaphore& VulkanRenderSync::getImageSemaphore() {
    return imageSemaphore;
}

VulkanSemaphore& VulkanRenderSync::getRenderSemaphore() {
    return renderSemaphore;
}