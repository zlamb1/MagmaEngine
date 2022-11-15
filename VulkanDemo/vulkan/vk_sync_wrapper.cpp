#include "vk_sync_wrapper.h";

// Public

VkSyncWrapper::VkSyncWrapper(VkDeviceWrapper& _vkDeviceWrapper) :
    vkDeviceWrapper{ _vkDeviceWrapper }
{

}

VkSyncWrapper::~VkSyncWrapper() {
    vkDestroySemaphore(vkDeviceWrapper.getLogicalDevice(), 
        vkImageSemaphore, nullptr);
    vkDestroySemaphore(vkDeviceWrapper.getLogicalDevice(), 
        vkRenderSemaphore, nullptr);
    vkDestroyFence(vkDeviceWrapper.getLogicalDevice(), vkFlightFence, nullptr);
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
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(vkDeviceWrapper.getLogicalDevice(),
        &semaphoreInfo, nullptr, &vkImageSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(vkDeviceWrapper.getLogicalDevice(), 
            &semaphoreInfo, nullptr, &vkRenderSemaphore) != VK_SUCCESS ||
        vkCreateFence(vkDeviceWrapper.getLogicalDevice(), 
            &fenceInfo, nullptr, &vkFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create semaphores!");
    }
}