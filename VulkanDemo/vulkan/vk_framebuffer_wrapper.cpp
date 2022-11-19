#include "vk_framebuffer_wrapper.h"

VkFramebufferWrapper::VkFramebufferWrapper(VkDeviceWrapper& _vkDeviceWrapper,
    VkSwapChainWrapper& _vkSwapChainWrapper,
    VkRenderPassWrapper& _vkRenderPassWrapper) :
    vkDeviceWrapper{ _vkDeviceWrapper },
	vkSwapChainWrapper{ _vkSwapChainWrapper },
    vkRenderPassWrapper{ _vkRenderPassWrapper }
{

}

VkFramebufferWrapper::~VkFramebufferWrapper() {
    for (auto framebuffer : vkFramebuffers) {
        vkDestroyFramebuffer(vkDeviceWrapper.vkDevice,
            framebuffer, nullptr);
    }
}

std::vector<VkFramebuffer>& VkFramebufferWrapper::getFramebuffers() {
    return vkFramebuffers;
}

void VkFramebufferWrapper::init() {
    auto logger = _VkLogger::Instance();

	vkFramebuffers.resize(vkSwapChainWrapper.getImageViews().size());

    for (size_t i = 0; i < vkSwapChainWrapper.getImageViews().size(); i++) {
        VkImageView attachments[] = {
            vkSwapChainWrapper.getImageViews()[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = vkRenderPassWrapper.getRenderPass();
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = vkSwapChainWrapper.getSwapChainExtent().width;
        framebufferInfo.height = vkSwapChainWrapper.getSwapChainExtent().height;
        framebufferInfo.layers = 1;

        auto vkCreateFramebufferResult = vkCreateFramebuffer(vkDeviceWrapper.vkDevice,
            &framebufferInfo, nullptr, &vkFramebuffers[i]);
        logger.LogResult("vkCreateFramebuffer =>", vkCreateFramebufferResult);
    }
}