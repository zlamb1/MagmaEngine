#include "vk_pipeline_wrapper.h"

// Public

VkPipelineWrapper::VkPipelineWrapper(VkDeviceWrapper& _vkDeviceWrapper,
	VkSwapChainWrapper& _vkSwapChainWrapper) :
	logger{ _VkLogger::Instance() }, 
	vkDeviceWrapper(_vkDeviceWrapper), 
	vkSwapChainWrapper{_vkSwapChainWrapper}
{
	initGraphicsPipeline();
	initFixedFunctionState();
	initRenderPass();
}

VkPipelineWrapper::~VkPipelineWrapper() {
	vkDestroyPipeline(vkDeviceWrapper.vkDevice, vkPipeline, nullptr);
}

void VkPipelineWrapper::newFrame(VkCmdBufferWrapper& vkCmdBufferWrapper, 
	uint32_t imageIndex) {
	vkCmdBufferWrapper.resetCmdBuffer();
	vkCmdBufferWrapper.recordCmdBuffer();

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

	vkCmdBeginRenderPass(vkCmdBufferWrapper.vkCmdBuffer, 
		&renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(vkCmdBufferWrapper.vkCmdBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapchainExtent.width;
	viewport.height = (float)swapchainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(vkCmdBufferWrapper.vkCmdBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapchainExtent;
	vkCmdSetScissor(vkCmdBufferWrapper.vkCmdBuffer, 0, 1, &scissor);

	vkCmdDraw(vkCmdBufferWrapper.vkCmdBuffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(vkCmdBufferWrapper.vkCmdBuffer);

	auto vkEndCommandBufferResult = vkEndCommandBuffer(vkCmdBufferWrapper.vkCmdBuffer);
	logger.LogResult("vkEndCommandBuffer =>", vkEndCommandBufferResult);
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

	auto vkCreateGraphicsPipelineResult = vkCreateGraphicsPipelines(vkDeviceWrapper.vkDevice,
		VK_NULL_HANDLE, 1, &vkPipelineInfo, nullptr,
		&vkPipeline);
	logger.LogResult("vkCreateGraphicsPipeline =>", vkCreateGraphicsPipelineResult);

	// initialize frame buffers after init
	initFramebuffers();
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