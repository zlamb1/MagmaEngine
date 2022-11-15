#include "vk_pipeline_wrapper.h"

// Public

VkPipelineWrapper::VkPipelineWrapper(VkDeviceWrapper& _vkDeviceWrapper,
	VkSwapChainWrapper& _vkSwapChainWrapper) :
	vkDeviceWrapper(_vkDeviceWrapper), vkSwapChainWrapper{ _vkSwapChainWrapper }
{
	initGraphicsPipeline();
	initFixedFunctionState();
	initRenderPass();
}

void VkPipelineWrapper::newFrame(VkCommandWrapper& vkCommandWrapper, uint32_t imageIndex) {
	vkCommandWrapper.resetCommandBuffer();
	vkCommandWrapper.recordCommandBuffer(imageIndex);

	auto swapchainExtent = vkSwapChainWrapper.getSwapChainExtent();

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = vkRenderPassWrapper->getRenderPass();
	renderPassInfo.framebuffer = 
		vkFramebufferWrapper->getFramebuffers()[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapchainExtent;

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(vkCommandWrapper.getCommandBuffer(), 
		&renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(vkCommandWrapper.getCommandBuffer(), 
		VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapchainExtent.width;
	viewport.height = (float)swapchainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(vkCommandWrapper.getCommandBuffer(), 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapchainExtent;
	vkCmdSetScissor(vkCommandWrapper.getCommandBuffer(), 0, 1, &scissor);

	vkCmdDraw(vkCommandWrapper.getCommandBuffer(), 3, 1, 0, 0);

	vkCmdEndRenderPass(vkCommandWrapper.getCommandBuffer());

	if (vkEndCommandBuffer(vkCommandWrapper.getCommandBuffer()) 
		!= VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}

void VkPipelineWrapper::init() {

	VkGraphicsPipelineCreateInfo vkPipelineInfo{};
	vkPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	vkPipelineInfo.stageCount = 2;

	auto shaderStages = vkGraphicsPipeline->getShaderStages();
	vkPipelineInfo.pStages = shaderStages.data();

	vkPipelineInfo.pVertexInputState = &vkFixedFunctionWrapper->getVertexInputState();
	vkPipelineInfo.pInputAssemblyState = &vkFixedFunctionWrapper->getInputAssemblyState();
	vkPipelineInfo.pViewportState = &vkFixedFunctionWrapper->getViewportState();
	vkPipelineInfo.pRasterizationState = &vkFixedFunctionWrapper->getRasterizerState();
	vkPipelineInfo.pMultisampleState = &vkFixedFunctionWrapper->getMultisamplingState();
	vkPipelineInfo.pDepthStencilState = &vkFixedFunctionWrapper->getDepthStencilState(); // Optional
	vkPipelineInfo.pColorBlendState = &vkFixedFunctionWrapper->getColorBlendState();
	vkPipelineInfo.pDynamicState = &vkFixedFunctionWrapper->getDynamicState();

	vkPipelineInfo.layout = vkFixedFunctionWrapper->getPipelineLayout();

	vkPipelineInfo.renderPass = vkRenderPassWrapper->getRenderPass();
	vkPipelineInfo.subpass = 0;

	vkPipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // optional
	vkPipelineInfo.basePipelineIndex = -1; // optional

	if (vkCreateGraphicsPipelines(vkDeviceWrapper.getLogicalDevice(), 
		VK_NULL_HANDLE, 1, &vkPipelineInfo, nullptr, 
		&vkPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}
	else {
		// initialize frame buffers after init
		initFramebuffers();
	}
}

// Private

void VkPipelineWrapper::initGraphicsPipeline() {
	vkGraphicsPipeline = std::make_unique<VkGraphicsPipeline>(vkDeviceWrapper);
	vkGraphicsPipeline->init();
}

void VkPipelineWrapper::initFixedFunctionState() {
	vkFixedFunctionWrapper = std::make_unique<VkFixedFunctionWrapper>(
		vkDeviceWrapper, vkSwapChainWrapper);
	vkFixedFunctionWrapper->init();
}

void VkPipelineWrapper::initRenderPass() {
	vkRenderPassWrapper = std::make_unique<VkRenderPassWrapper>(
		vkDeviceWrapper, vkSwapChainWrapper);
	vkRenderPassWrapper->init();
}

void VkPipelineWrapper::initFramebuffers() {
	vkFramebufferWrapper = std::make_unique<VkFramebufferWrapper>(
		vkDeviceWrapper, vkSwapChainWrapper, *vkRenderPassWrapper);
	vkFramebufferWrapper->init();
}