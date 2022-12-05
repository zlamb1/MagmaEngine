#include "vulkan_framebuffer.h"

namespace Magma {

    VulkanFramebuffer::VulkanFramebuffer(std::shared_ptr<VulkanDevice> pVulkanDevice,
        std::shared_ptr<VulkanSwapchain> pVulkanSwapchain) : pVulkanDevice{ pVulkanDevice },
        pVulkanSwapchain{ pVulkanSwapchain } {}

    VulkanFramebuffer::~VulkanFramebuffer() {
        destroyFramebuffers();
    }

    VkResult VulkanFramebuffer::init() {
        if (pVulkanDevice == nullptr) {
            Z_LOG_TXT("VulkanFramebuffer::init", "pVulkanDevice is nullptr");
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        if (pVulkanSwapchain == nullptr) {
            Z_LOG_TXT("VulkanFramebuffer::init", "pVulkanSwapchain is nullptr");
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        if (pVulkanRenderPass == nullptr) {
            Z_LOG_TXT("VulkanFramebuffer::init", "pVulkanRenderPass is nullptr");
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        vkFramebuffers.resize(pVulkanSwapchain->getImageViews().size());

        for (size_t i = 0; i < pVulkanSwapchain->getImageViews().size(); i++) {
            std::vector<VkImageView> attachments{
                pVulkanSwapchain->getImageViews()[i],
                pDepthImageView->getImageView()
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = pVulkanRenderPass->getRenderPass();
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = pVulkanSwapchain->getSwapchainExtent().width;
            framebufferInfo.height = pVulkanSwapchain->getSwapchainExtent().height;
            framebufferInfo.layers = 1;

            auto createFramebufferResult = vkCreateFramebuffer(pVulkanDevice->getDevice(), &framebufferInfo,
                nullptr, &vkFramebuffers[i]);
            Z_LOG_OBJ("VulkanFramebuffer::init::vkCreateFramebuffer", createFramebufferResult);
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

}