#include "vk_pipeline_components.h"

// _VkShaderPipeline Implementation

_VkShaderPipeline::_VkShaderPipeline() {

}

_VkShaderPipeline::~_VkShaderPipeline() {
	for (auto _vkShader : _vkShaders) {
		delete _vkShader;
	}

	_vkShaders.clear();
}

std::vector<VkPipelineShaderStageCreateInfo> _VkShaderPipeline::getShaderStages() {
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};
	for (auto _vkShader : _vkShaders) {
		if (_vkShader != nullptr) {
			shaderStages.push_back(_vkShader->vkShaderStageInfo);
		}
	}

	return shaderStages;
}

void _VkShaderPipeline::addShader(_VkShaderInfo _vkShaderInfo) {
	_vkShaderInfos.push_back(_vkShaderInfo);
}

VkResult _VkShaderPipeline::create() {
	auto _vkLogger = _VkLogger::Instance();

	if (_pDevice == nullptr) {
		_vkLogger.LogText("_VkShaderPipeline{createShader} => _pDevice is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	for (auto _vkShaderInfo : _vkShaderInfos) {
		// shader init is done in constructor
		_VkShader* _vkShader = new _VkShader();
		_vkShader->pDevice = &_pDevice->vkDevice;
		_vkShader->pShaderCode = _vkShaderInfo.pCode;
		_vkShader->pShaderType = (shaderc_shader_kind)_vkShaderInfo.pShaderType;
		_vkShader->create();
		_vkShaders.push_back(_vkShader);
	}

	return VK_SUCCESS;
}

// _VkFixedFunctionState Implementation

_VkFixedFunctionState::_VkFixedFunctionState() {

}

_VkFixedFunctionState::~_VkFixedFunctionState() {
	if (_pDevice != nullptr) {
		vkDestroyPipelineLayout(_pDevice->vkDevice, vkPipelineLayout, nullptr);
	}
}

VkResult _VkFixedFunctionState::create() {
	auto _vkLogger = _VkLogger::Instance();

	if (_pDevice == nullptr) {
		_vkLogger.LogText("_VkFixedFunctionState => _pDevice is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	if (_pSwapchain == nullptr) {
		_vkLogger.LogText("_VkFixedFunctionState => _pSwapchain is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	vkDynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	vkDynamicState.dynamicStateCount = static_cast<uint32_t>(vkDynamicStates.size());
	vkDynamicState.pDynamicStates = vkDynamicStates.data();

	vkVertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vkVertexInputState.vertexBindingDescriptionCount = vertexBindingDescriptionCount;
	vkVertexInputState.pVertexBindingDescriptions = pVertexBindingDescriptions; // optional
	vkVertexInputState.vertexAttributeDescriptionCount = vertexAttributeDescriptionCount;
	vkVertexInputState.pVertexAttributeDescriptions = pVertexAttributeDescriptions; // optional

	vkInputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	vkInputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	vkInputAssemblyState.primitiveRestartEnable = VK_FALSE;

	auto vkSwapchainExtent = _pSwapchain->vkSwapchainExtent;
	vkViewport.x = 0.0f;
	vkViewport.y = 0.0f;
	vkViewport.width = (float)vkSwapchainExtent.width;
	vkViewport.height = (float)vkSwapchainExtent.height;
	vkViewport.minDepth = 0.0f;
	vkViewport.maxDepth = 1.0f;
	vkRect2D.offset = { 0, 0 };
	vkRect2D.extent = vkSwapchainExtent;
	vkViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	vkViewportState.viewportCount = 1;
	vkViewportState.pViewports = &vkViewport;
	vkViewportState.scissorCount = 1;
	vkViewportState.pScissors = &vkRect2D;

	vkRasterizerState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	vkRasterizerState.depthClampEnable = VK_FALSE;
	vkRasterizerState.rasterizerDiscardEnable = VK_FALSE;
	vkRasterizerState.polygonMode = VK_POLYGON_MODE_FILL;
	vkRasterizerState.lineWidth = 1.0f;
	vkRasterizerState.cullMode = VK_CULL_MODE_BACK_BIT;
	vkRasterizerState.frontFace = VK_FRONT_FACE_CLOCKWISE;
	vkRasterizerState.depthBiasEnable = VK_FALSE;
	vkRasterizerState.depthBiasConstantFactor = 0.0f; // optional
	vkRasterizerState.depthBiasClamp = 0.0f; // optional
	vkRasterizerState.depthBiasSlopeFactor = 0.0f; // optional

	vkMultisamplingState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	vkMultisamplingState.sampleShadingEnable = VK_FALSE;
	vkMultisamplingState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	vkMultisamplingState.minSampleShading = 1.0f; // optional
	vkMultisamplingState.pSampleMask = nullptr; // optional
	vkMultisamplingState.alphaToCoverageEnable = VK_FALSE; // optional
	vkMultisamplingState.alphaToOneEnable = VK_FALSE; // optional

	vkColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT 
		| VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	vkColorBlendAttachmentState.blendEnable = VK_FALSE;
	vkColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // optional
	vkColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // optional
	vkColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD; // optional
	vkColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // optional
	vkColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // optional
	vkColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD; // optional

	vkColorBlendingState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	vkColorBlendingState.logicOpEnable = VK_FALSE;
	vkColorBlendingState.logicOp = VK_LOGIC_OP_COPY; // optional
	vkColorBlendingState.attachmentCount = 1;
	vkColorBlendingState.pAttachments = &vkColorBlendAttachmentState;
	vkColorBlendingState.blendConstants[0] = 0.0f; // optional
	vkColorBlendingState.blendConstants[1] = 0.0f; // optional
	vkColorBlendingState.blendConstants[2] = 0.0f; // optional
	vkColorBlendingState.blendConstants[3] = 0.0f; // optional

	vkPipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	vkPipelineLayoutInfo.setLayoutCount = 0; // optional
	vkPipelineLayoutInfo.pSetLayouts = nullptr; // optional
	vkPipelineLayoutInfo.pushConstantRangeCount = 0; // optional
	vkPipelineLayoutInfo.pPushConstantRanges = nullptr; // optional

	auto vkCreatePipelineLayoutResult = vkCreatePipelineLayout(_pDevice->vkDevice, 
		&vkPipelineLayoutInfo, nullptr, &vkPipelineLayout);
	_vkLogger.LogResult("vkCreatePipelineLayout =>", vkCreatePipelineLayoutResult);

	return vkCreatePipelineLayoutResult;
}

// _VkRenderPass Implementation

_VkRenderPass::_VkRenderPass() {

}

_VkRenderPass::~_VkRenderPass() {
	if (_pDevice != nullptr) {
		vkDestroyRenderPass(_pDevice->vkDevice, vkRenderPass, nullptr);
	}
}

VkResult _VkRenderPass::create() {
	auto _vkLogger = _VkLogger::Instance();

	if (_pDevice == nullptr) {
		_vkLogger.LogText("_VkRenderPass => _pDevice is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	if (_pSwapchain == nullptr) {
		_vkLogger.LogText("_VkRenderPass => _pSwapchain is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = _pSwapchain->vkSwapchainImageFormat;
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

	auto vkCreateRenderPassResult = vkCreateRenderPass(_pDevice->vkDevice, &renderPassInfo, 
		nullptr, &vkRenderPass);
	_vkLogger.LogResult("vkCreateRenderPass =>", vkCreateRenderPassResult);
	
	return vkCreateRenderPassResult;
}