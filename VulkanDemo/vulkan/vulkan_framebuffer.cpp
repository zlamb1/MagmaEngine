#include "vulkan_framebuffer.h"

VulkanFramebuffer::VulkanFramebuffer(std::shared_ptr<VulkanDevice> pVulkanDevice,
    std::shared_ptr<VulkanSwapchain> pVulkanSwapchain) : pVulkanDevice{ pVulkanDevice },
    pVulkanSwapchain{ pVulkanSwapchain } {}

VulkanFramebuffer::~VulkanFramebuffer() {
    destroyFramebuffers();
}

VkResult VulkanFramebuffer::init() {    
    if (pVulkanDevice == nullptr) {
        VulkanLogger::instance().enqueueText("VulkanFramebuffer::init", "pVulkanDevice is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    if (pVulkanSwapchain == nullptr) {
        VulkanLogger::instance().enqueueText("VulkanFramebuffer::init", "pVulkanSwapchain is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    if (pVulkanRenderPass == nullptr) {
        VulkanLogger::instance().enqueueText("VulkanFramebuffer::init", "pVulkanRenderPass is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

	vkFramebuffers.resize(pVulkanSwapchain->getImageViews().size());

    for (size_t i = 0; i < pVulkanSwapchain->getImageViews().size(); i++) {
        VkImageView attachments[] = {
            pVulkanSwapchain->getImageViews()[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = pVulkanRenderPass->getRenderPass();
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = pVulkanSwapchain->getSwapchainExtent().width;
        framebufferInfo.height = pVulkanSwapchain->getSwapchainExtent().height;
        framebufferInfo.layers = 1;

        auto createFramebufferResult = vkCreateFramebuffer(pVulkanDevice->getDevice(), &framebufferInfo,
            nullptr, &vkFramebuffers[i]);
        VulkanLogger::instance().enqueueObject("VulkanFramebuffer::init::vkCreateFramebuffer", 
            createFramebufferResult);
    }
    
    return VK_SUCCESS;
}

std::vector<VkFramebuffer>& VulkanFramebuffer::getFramebuffers() {
    return vkFramebuffers;
}

void VulkanFramebuffer::destroyFramebuffers() {
    if (pVulkanDevice != nullptr) {
        for (auto vkFramebuffer : vkFramebuffers) {
            vkDestroyFramebuffer(pVulkanDevice->getDevice(), vkFramebuffer, nullptr);
        }
        vkFramebuffers.clear();
    }
}