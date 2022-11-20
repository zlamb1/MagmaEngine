#include "vk_pipeline_wrapper.h"

_VkPipeline::_VkPipeline() : _vkLogger{ _VkLogger::Instance() } {

}

_VkPipeline::~_VkPipeline() {
	vkDestroyPipeline(_pDevice->vkDevice, vkPipeline, nullptr);
}

void _VkPipeline::onNewFrame(_VkCmdBuffer& vkCmdBuffer, uint32_t imageIndex) {
	vkCmdBuffer.resetCmdBuffer();
	vkCmdBuffer.recordCmdBuffer();

	auto vkSwapchainExtent = _pSwapchain->vkSwapchainExtent;

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = vkRenderPassWrapper->getRenderPass();
	renderPassInfo.framebuffer = _vkFramebuffer->vkFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = vkSwapchainExtent;

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(vkCmdBuffer.vkCmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(vkCmdBuffer.vkCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)vkSwapchainExtent.width;
	viewport.height = (float)vkSwapchainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(vkCmdBuffer.vkCmdBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = vkSwapchainExtent;
	vkCmdSetScissor(vkCmdBuffer.vkCmdBuffer, 0, 1, &scissor);

	vkCmdDraw(vkCmdBuffer.vkCmdBuffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(vkCmdBuffer.vkCmdBuffer);

	auto vkEndCommandBufferResult = vkEndCommandBuffer(vkCmdBuffer.vkCmdBuffer);
	_vkLogger.LogResult("vkEndCommandBuffer =>", vkEndCommandBufferResult);
}

VkResult _VkPipeline::create() {
	if (_pDevice == nullptr) {
		_vkLogger.LogText("_VkPipeline => _pDevice is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	if (_pSwapchain == nullptr) {
		_vkLogger.LogText("_VkPipeline => _pSwapchain is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	// member init
	_vkShaderPipeline = std::make_unique<_VkShaderPipeline>();

	_vkShaderPipeline->_pDevice = _pDevice;
	_vkShaderPipeline->pAllocator = pAllocator;

	_vkShaderPipeline->create();

	_vkFixedFunctionState = std::make_unique<_VkFixedFunctionState>();

	_vkFixedFunctionState->_pDevice = _pDevice;
	_vkFixedFunctionState->_pSwapchain = _pSwapchain;
	_vkFixedFunctionState->pAllocator = pAllocator;

	_vkFixedFunctionState->create();

	vkRenderPassWrapper = std::make_unique<VkRenderPassWrapper>(*_pDevice, *_pSwapchain);
	vkRenderPassWrapper->init();

	// pipeline init
	VkGraphicsPipelineCreateInfo vkPipelineInfo{};
	vkPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	vkPipelineInfo.stageCount = 2;

	auto shaderStages = _vkShaderPipeline->getShaderStages();
	vkPipelineInfo.pStages = shaderStages.data();

	vkPipelineInfo.pVertexInputState = &_vkFixedFunctionState->vkVertexInputState;
	vkPipelineInfo.pInputAssemblyState = &_vkFixedFunctionState->vkInputAssemblyState;
	vkPipelineInfo.pViewportState = &_vkFixedFunctionState->vkViewportState;
	vkPipelineInfo.pRasterizationState = &_vkFixedFunctionState->vkRasterizerState;
	vkPipelineInfo.pMultisampleState = &_vkFixedFunctionState->vkMultisamplingState;
	vkPipelineInfo.pDepthStencilState = &_vkFixedFunctionState->vkDepthStencilState; // Optional
	vkPipelineInfo.pColorBlendState = &_vkFixedFunctionState->vkColorBlendingState;
	vkPipelineInfo.pDynamicState = &_vkFixedFunctionState->vkDynamicState;

	vkPipelineInfo.layout = _vkFixedFunctionState->vkPipelineLayout;

	vkPipelineInfo.renderPass = vkRenderPassWrapper->getRenderPass();
	vkPipelineInfo.subpass = 0;

	vkPipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // optional
	vkPipelineInfo.basePipelineIndex = -1; // optional

	auto vkCreateGraphicsPipelineResult = vkCreateGraphicsPipelines(_pDevice->vkDevice,
		VK_NULL_HANDLE, 1, &vkPipelineInfo, nullptr, &vkPipeline);
	_vkLogger.LogResult("vkCreateGraphicsPipeline =>", vkCreateGraphicsPipelineResult);
	if (vkCreateGraphicsPipelineResult != VK_SUCCESS) {
		return vkCreateGraphicsPipelineResult;
	}

	// framebuffer init
	_vkFramebuffer = std::make_unique<_VkFramebuffer>();

	_vkFramebuffer->_pSwapchain = _pSwapchain;
	_vkFramebuffer->pDevice = &_pDevice->vkDevice;
	_vkFramebuffer->pRenderPass = &vkRenderPassWrapper->getRenderPass();

	auto vkFramebufferResult = _vkFramebuffer->create();
	if (vkFramebufferResult != VK_SUCCESS) {
		return vkFramebufferResult;
	}

	return VK_SUCCESS;
}