#include "vk_render_pass_wrapper.h"

// Public

VkRenderPassWrapper::VkRenderPassWrapper(_VkDevice& _vkDevice,
    _VkSwapchain& _vkSwapchain) :
    _vkDevice{ _vkDevice },
    _vkSwapchain{ _vkSwapchain }
{

}

VkRenderPassWrapper::~VkRenderPassWrapper() {
    vkDestroyRenderPass(_vkDevice.vkDevice,
        vkRenderPass, nullptr);
}

void VkRenderPassWrapper::init() {
    auto logger = _VkLogger::Instance();

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = _vkSwapchain.vkSwapchainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;


    auto vkCreateRenderPassResult = vkCreateRenderPass(_vkDevice.vkDevice,
        &renderPassInfo, nullptr, &vkRenderPass);
    logger.LogResult("vkCreateRenderPass =>", vkCreateRenderPassResult);
}

VkRenderPass& VkRenderPassWrapper::getRenderPass() {
    return vkRenderPass;
}