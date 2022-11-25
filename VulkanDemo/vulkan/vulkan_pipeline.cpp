#include "vulkan_pipeline.h"

VulkanPipeline::~VulkanPipeline() {
	delete vulkanFramebuffer;
	if (pDevice != nullptr) {
		vkDestroyPipeline(pDevice->getDevice(), vkPipeline, nullptr);
	}
}

VkResult VulkanPipeline::init() {
	if (pDevice == nullptr) {
		VulkanLogger::instance().enqueueText("VulkanPipeline::init", "pDevice is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	if (pSwapchain == nullptr) {
		VulkanLogger::instance().enqueueText("VulkanPipeline::init", "pSwapchain is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	// member init
	vulkanShaderPipeline.pDevice = pDevice;
	vulkanShaderPipeline.pAllocator = pAllocator;
	vulkanShaderPipeline.init();

	vulkanFixedFuctionState = std::make_unique<VulkanFixedFunctionState>();
	vulkanFixedFuctionState->pDevice = pDevice;
	vulkanFixedFuctionState->pSwapchain = pSwapchain;
	vulkanFixedFuctionState->pAllocator = pAllocator;

	for (auto& _vkBindingDescription : pBindingDescriptions) {
		vulkanFixedFuctionState->pVertexBindingDescriptions.push_back(_vkBindingDescription);
	}
	for (auto& _vkAttributeDescription : pAttributeDescriptions) {
		vulkanFixedFuctionState->pVertexAttributeDescriptions.push_back(_vkAttributeDescription);
	}
	vulkanFixedFuctionState->init();

	vulkanRenderPass = std::make_unique<VulkanRenderPass>();
	vulkanRenderPass->pDevice = pDevice;
	vulkanRenderPass->pSwapchain = pSwapchain;
	vulkanRenderPass->init();

	// pipeline init
	VkGraphicsPipelineCreateInfo vkPipelineInfo{};
	vkPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	vkPipelineInfo.stageCount = static_cast<uint32_t>(vulkanShaderPipeline.getShaderStages().size());
	
	auto shaderStages = vulkanShaderPipeline.getShaderStages();
	vkPipelineInfo.pStages = shaderStages.data();

	vkPipelineInfo.pVertexInputState = &vulkanFixedFuctionState->getVertexInputCreateInfo();
	vkPipelineInfo.pInputAssemblyState = &vulkanFixedFuctionState->getInputAssemblyCreateInfo();
	vkPipelineInfo.pViewportState = &vulkanFixedFuctionState->getViewportCreateInfo();
	vkPipelineInfo.pRasterizationState = &vulkanFixedFuctionState->getRasterizationCreateInfo();
	vkPipelineInfo.pMultisampleState = &vulkanFixedFuctionState->getMultisampleCreateInfo();
	vkPipelineInfo.pDepthStencilState = &vulkanFixedFuctionState->getDepthStencilCreateInfo(); // Optional
	vkPipelineInfo.pColorBlendState = &vulkanFixedFuctionState->getColorBlendCreateInfo();
	vkPipelineInfo.pDynamicState = &vulkanFixedFuctionState->getDynamicCreateInfo();

	vkPipelineInfo.layout = vulkanFixedFuctionState->getPipelineLayout();

	vkPipelineInfo.renderPass = vulkanRenderPass->getRenderPass();
	vkPipelineInfo.subpass = 0;

	vkPipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // optional
	vkPipelineInfo.basePipelineIndex = -1; // optional

	auto createGraphicsPipelineResult = vkCreateGraphicsPipelines(pDevice->getDevice(),
		VK_NULL_HANDLE, 1, &vkPipelineInfo, nullptr, &vkPipeline);
	VulkanLogger::instance().enqueueObject("VulkanPipeline::init::vkCreateGraphicsPipeline", 
		createGraphicsPipelineResult);
	if (createGraphicsPipelineResult != VK_SUCCESS) {
		return createGraphicsPipelineResult;
	}

	// framebuffer init
	auto vkFramebufferResult = initFramebuffers();
	if (vkFramebufferResult != VK_SUCCESS) {
		return vkFramebufferResult;
	}

	return VK_SUCCESS;
}

VkResult VulkanPipeline::initFramebuffers() {
	vulkanFramebuffer = new VulkanFramebuffer();
	vulkanFramebuffer->pSwapchain = pSwapchain;
	vulkanFramebuffer->pDevice = &pDevice->getDevice();
	vulkanFramebuffer->pRenderPass = &vulkanRenderPass->getRenderPass();
	return vulkanFramebuffer->init();
}

void VulkanPipeline::addShader(VulkanShader* vulkanShaderHandle) {
	vulkanShaderPipeline.addShader(vulkanShaderHandle);
}

void VulkanPipeline::deleteFramebuffers() {
	if (vulkanFramebuffer != nullptr) {
		delete vulkanFramebuffer;
		vulkanFramebuffer = nullptr;
	}
}

void VulkanPipeline::onNewFrame(VulkanCmdBuffer& vulkanCmdBuffer, uint32_t imageIndex,
	uint32_t vertexCount) {
	vulkanCmdBuffer.reset();
	vulkanCmdBuffer.record();

	auto vkSwapchainExtent = pSwapchain->getSwapchainExtent();

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

	if (pBuffers.size() > 0) {
		VkDeviceSize offsets[] = { 0 };
		VkBuffer vertexBuffers[] = { pBuffers[0]->getBuffer() };
		vkCmdBindVertexBuffers(vulkanCmdBuffer.getCmdBuffer(), 0, 1, vertexBuffers, offsets);
		vkCmdDraw(vulkanCmdBuffer.getCmdBuffer(), vertexCount, 1, 0, 0);
	}

	vkCmdEndRenderPass(vulkanCmdBuffer.getCmdBuffer());

	auto endCommandBufferResult = vkEndCommandBuffer(vulkanCmdBuffer.getCmdBuffer());
	VulkanLogger::instance().enqueueObject("VulkanPipeline::onNewFrame::vkEndCommandBuffer",
		endCommandBufferResult);
}