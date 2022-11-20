#include "vk_sync_wrapper.h";

// _VkSemaphore Implementation

_VkSemaphore::_VkSemaphore() {

}

_VkSemaphore::~_VkSemaphore() {
    if (pDevice != nullptr) {
        vkDestroySemaphore(*pDevice, vkSemaphore, nullptr);
    }
}

VkResult _VkSemaphore::create() {
    auto _vkLogger = _VkLogger::Instance();

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (pDevice == nullptr) {
        _vkLogger.LogText("_VkSemaphore => pDevice is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    return vkCreateSemaphore(*pDevice, &semaphoreInfo, nullptr, &vkSemaphore);
}

// _VkFence Implementation

_VkFence::_VkFence() {

}

_VkFence::~_VkFence() {
    if (pDevice != nullptr) {
        vkDestroyFence(*pDevice, vkFence, pAllocator);
    }
}

VkResult _VkFence::create() {
    auto _vkLogger = _VkLogger::Instance();
    
    if (pDevice == nullptr) {
        _vkLogger.LogText("_VkFence => pDevice is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    return vkCreateFence(*pDevice, &fenceInfo, pAllocator, &vkFence);
}

// _VkRenderSync Implementation

_VkRenderSync::_VkRenderSync() {

}

VkResult _VkRenderSync::create() {
    auto _vkLogger = _VkLogger::Instance();

    if (pDevice == nullptr) {
        _vkLogger.LogText("_VkRenderSync => pDevice is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    _vkFlightFence.pDevice = pDevice;
    auto vkFlightFenceResult = _vkFlightFence.create();
    _vkLogger.LogResult("vkFlightFence =>", vkFlightFenceResult);
    if (vkFlightFenceResult != VK_SUCCESS) {
        return vkFlightFenceResult;
    }

    _vkImageSemaphore.pDevice = pDevice;
    auto vkImageSemaphoreResult = _vkImageSemaphore.create();
    _vkLogger.LogResult("vkImageSemaphore =>", vkImageSemaphoreResult);
    if (vkImageSemaphoreResult != VK_SUCCESS) {
        return vkImageSemaphoreResult;
    }

    _vkRenderSemaphore.pDevice = pDevice;
    auto vkRenderSemaphoreResult = _vkRenderSemaphore.create();
    _vkLogger.LogResult("vkRenderSemaphore =>", vkRenderSemaphoreResult);
    if (vkRenderSemaphoreResult != VK_SUCCESS) {
        return vkRenderSemaphoreResult;
    }

    return VK_SUCCESS;
}