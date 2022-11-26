#include "vulkan_pipeline.h"

VulkanPipeline::VulkanPipeline(std::shared_ptr<VulkanSwapchain> pVulkanSwapchain) :
	pVulkanSwapchain{ pVulkanSwapchain } {}

VulkanPipeline::~VulkanPipeline() {
	if (pVulkanSwapchain != nullptr && pVulkanSwapchain->pVulkanDevice != nullptr) {
		vkDestroyPipeline(pVulkanSwapchain->pVulkanDevice->getDevice(), vkPipeline, nullptr);
	}
}

VkResult VulkanPipeline::init() {
	if (pVulkanSwapchain == nullptr) {
		VulkanLogger::instance().enqueueText("VulkanPipeline::init", "pVulkanSwapchain is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	auto pVulkanDevice = pVulkanSwapchain->pVulkanDevice;

	// member init
	vulkanShaderPipeline.pAllocator = pAllocator;
	vulkanShaderPipeline.init();

	vulkanFixedFuctionState = std::make_unique<VulkanFixedFunctionState>(pVulkanDevice, pVulkanSwapchain);
	vulkanFixedFuctionState->pAllocator = pAllocator;

	for (auto& _vkBindingDescription : pBindingDescriptions) {
		vulkanFixedFuctionState->pVertexBindingDescriptions.push_back(_vkBindingDescription);
	}
	for (auto& _vkAttributeDescription : pAttributeDescriptions) {
		vulkanFixedFuctionState->pVertexAttributeDescriptions.push_back(_vkAttributeDescription);
	}
	vulkanFixedFuctionState->init();

	vulkanRenderPass = std::make_unique<VulkanRenderPass>(pVulkanDevice, pVulkanSwapchain);
	vulkanRenderPass->init();

	// pipeline init
	VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stageCount = static_cast<uint32_t>(vulkanShaderPipeline.getShaderStages().size());
	
	auto shaderStages = vulkanShaderPipeline.getShaderStages();
	pipelineCreateInfo.pStages = shaderStages.data();

	pipelineCreateInfo.pVertexInputState = &vulkanFixedFuctionState->getVertexInputCreateInfo();
	pipelineCreateInfo.pInputAssemblyState = &vulkanFixedFuctionState->getInputAssemblyCreateInfo();
	pipelineCreateInfo.pViewportState = &vulkanFixedFuctionState->getViewportCreateInfo();
	pipelineCreateInfo.pRasterizationState = &vulkanFixedFuctionState->getRasterizationCreateInfo();
	pipelineCreateInfo.pMultisampleState = &vulkanFixedFuctionState->getMultisampleCreateInfo();
	pipelineCreateInfo.pDepthStencilState = &vulkanFixedFuctionState->getDepthStencilCreateInfo(); // Optional
	pipelineCreateInfo.pColorBlendState = &vulkanFixedFuctionState->getColorBlendCreateInfo();
	pipelineCreateInfo.pDynamicState = &vulkanFixedFuctionState->getDynamicCreateInfo();

	pipelineCreateInfo.layout = vulkanFixedFuctionState->getPipelineLayout();

	pipelineCreateInfo.renderPass = vulkanRenderPass->getRenderPass();
	pipelineCreateInfo.subpass = 0;

	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; // optional
	pipelineCreateInfo.basePipelineIndex = -1; // optional

	auto createGraphicsPipelineResult = vkCreateGraphicsPipelines(pVulkanDevice->getDevice(),
		VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &vkPipeline);
	VulkanLogger::instance().enqueueObject("VulkanPipeline::init::vkCreateGraphicsPipeline", 
		createGraphicsPipelineResult);
	if (createGraphicsPipelineResult != VK_SUCCESS) {
		return createGraphicsPipelineResult;
	}

	return initFramebuffers();
}

VkResult VulkanPipeline::initFramebuffers() {
	auto pVulkanDevice = pVulkanSwapchain->pVulkanDevice;
	if (vulkanFramebuffer == nullptr) {
		vulkanFramebuffer = std::make_shared<VulkanFramebuffer>(pVulkanDevice, pVulkanSwapchain);
	}
	vulkanFramebuffer->pVulkanRenderPass = vulkanRenderPass;
	return vulkanFramebuffer->init();
}

void VulkanPipeline::addShader(std::shared_ptr<VulkanShader> pVulkanShader) {
	vulkanShaderPipeline.addShader(pVulkanShader);
}

void VulkanPipeline::destroyFramebuffers() {
	if (vulkanFramebuffer != nullptr) {
		vulkanFramebuffer->destroyFramebuffers();
	}
}

void VulkanPipeline::onNewFrame(VulkanCmdBuffer& vulkanCmdBuffer, uint32_t imageIndex) {
	vulkanCmdBuffer.reset();
	vulkanCmdBuffer.record();

	auto vkSwapchainExtent = pVulkanSwapchain->getSwapchainExtent();

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = vulkanRenderPass->getRenderPass();
	renderPassInfo.framebuffer = vulkanFramebuffer->getFramebuffers()[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = vkSwapchainExtent;

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(vulkanCmdBuffer.getCmdBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(vulkanCmdBuffer.getCmdBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);

	vkCmdSetViewport(vulkanCmdBuffer.getCmdBuffer(), 0, 1, &vulkanFixedFuctionState->getViewport());
	vkCmdSetScissor(vulkanCmdBuffer.getCmdBuffer(), 0, 1, &vulkanFixedFuctionState->getScissor());

	if (pVulkanDrawer != nullptr) {
		pVulkanDrawer->onNewFrame(vulkanCmdBuffer);
	}

	vkCmdEndRenderPass(vulkanCmdBuffer.getCmdBuffer());

	auto endCommandBufferResult = vkEndCommandBuffer(vulkanCmdBuffer.getCmdBuffer());
	VulkanLogger::instance().enqueueObject("VulkanPipeline::onNewFrame::vkEndCommandBuffer",
		endCommandBufferResult);
}