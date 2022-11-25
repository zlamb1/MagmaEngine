#include "vulkan_framebuffer.h"

VulkanFramebuffer::~VulkanFramebuffer() {
    if (pDevice != nullptr) {
        for (auto vkFramebuffer : vkFramebuffers) {
            vkDestroyFramebuffer(*pDevice, vkFramebuffer, nullptr);
        }
    }
}

VkResult VulkanFramebuffer::init() {    
    if (pSwapchain == nullptr) {
        VulkanLogger::instance().enqueueText("VulkanFramebuffer::init", "pSwapchain is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    if (pDevice == nullptr) {
        VulkanLogger::instance().enqueueText("VulkanFramebuffer::init", "pDevice is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    if (pRenderPass == nullptr) {
        VulkanLogger::instance().enqueueText("VulkanFramebuffer::init", "pRenderPass is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

	vkFramebuffers.resize(pSwapchain->getImageViews().size());

    for (size_t i = 0; i < pSwapchain->getImageViews().size(); i++) {
        VkImageView attachments[] = {
            pSwapchain->getImageViews()[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = *pRenderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = pSwapchain->getSwapchainExtent().width;
        framebufferInfo.height = pSwapchain->getSwapchainExtent().height;
        framebufferInfo.layers = 1;

        auto createFramebufferResult = vkCreateFramebuffer(*pDevice,
            &framebufferInfo, nullptr, &vkFramebuffers[i]);
        VulkanLogger::instance().enqueueObject("VulkanFramebuffer::init::vkCreateFramebuffer", 
            createFramebufferResult);
    }
    
    return VK_SUCCESS;
}

std::vector<VkFramebuffer>& VulkanFramebuffer::getFramebuffers() {
    return vkFramebuffers;
}