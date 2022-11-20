#include "vk_framebuffer_wrapper.h"

_VkFramebuffer::_VkFramebuffer() {

}

_VkFramebuffer::~_VkFramebuffer() {
    if (pDevice != nullptr) {
        for (auto vkFramebuffer : vkFramebuffers) {
            vkDestroyFramebuffer(*pDevice, vkFramebuffer, nullptr);
        }
    }
}

VkResult _VkFramebuffer::create() {
    auto logger = _VkLogger::Instance();
    
    if (_pSwapchain == nullptr) {
        logger.LogText("_VkFramebuffer => pSwapchainWrapper is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    if (pDevice == nullptr) {
        logger.LogText("_VkFramebuffer => pDevice is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    if (pRenderPass == nullptr) {
        logger.LogText("_VkFramebuffer => pRenderPass is nullptr");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

	vkFramebuffers.resize(_pSwapchain->vkImageViews.size());

    for (size_t i = 0; i < _pSwapchain->vkImageViews.size(); i++) {
        VkImageView attachments[] = {
            _pSwapchain->vkImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = *pRenderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = _pSwapchain->vkSwapchainExtent.width;
        framebufferInfo.height = _pSwapchain->vkSwapchainExtent.height;
        framebufferInfo.layers = 1;

        auto vkCreateFramebufferResult = vkCreateFramebuffer(*pDevice,
            &framebufferInfo, nullptr, &vkFramebuffers[i]);
        logger.LogResult("vkCreateFramebuffer =>", vkCreateFramebufferResult);
    }
    
    return VK_SUCCESS;
}