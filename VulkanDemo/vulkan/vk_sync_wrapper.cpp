#include "vk_sync_wrapper.h";

// Public

VkSyncWrapper::VkSyncWrapper(VkDeviceWrapper& _vkDeviceWrapper) :
    vkDeviceWrapper{ _vkDeviceWrapper }
{

}

VkSyncWrapper::~VkSyncWrapper() {
    vkDestroySemaphore(vkDeviceWrapper.vkDevice,
        vkImageSemaphore, nullptr);
    vkDestroySemaphore(vkDeviceWrapper.vkDevice,
        vkRenderSemaphore, nullptr);
    vkDestroyFence(vkDeviceWrapper.vkDevice, vkFlightFence, nullptr);
}

VkFence& VkSyncWrapper::getFlightFence() {
    return vkFlightFence;
}

VkSemaphore& VkSyncWrapper::getImageSemaphore() {
    return vkImageSemaphore;
}

VkSemaphore& VkSyncWrapper::getRenderSemaphore() {
    return vkRenderSemaphore;
}

void VkSyncWrapper::init() {
    auto logger = _VkLogger::Instance();

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    logger.LogResult("vkImageSemaphore =>", vkCreateSemaphore(vkDeviceWrapper.vkDevice,
        &semaphoreInfo, nullptr, &vkImageSemaphore));
    logger.LogResult("vkImageSemaphore =>", vkCreateSemaphore(vkDeviceWrapper.vkDevice,
        &semaphoreInfo, nullptr, &vkRenderSemaphore));
    logger.LogResult("vkCreateFence =>", vkCreateFence(vkDeviceWrapper.vkDevice,
        &fenceInfo, nullptr, &vkFlightFence));
}