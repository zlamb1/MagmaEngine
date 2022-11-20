#include "vk_pipeline_components.h"

const char* vertexShaderCode = R"(#version 450
layout(location = 0) out vec3 fragColor;
vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);
vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);
void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragColor = colors[gl_VertexIndex];
})";

const char* fragmentShaderCode = R"(#version 450
layout(location = 0) in vec3 fragColor;
layout(location = 0) out vec4 outColor;
void main() {
    outColor = vec4(fragColor, 1.0);
})";

// _VkShaderPipeline Implementation

_VkShaderPipeline::_VkShaderPipeline() {

}

_VkShaderPipeline::~_VkShaderPipeline() {
	for (auto vkShaderWrapper : vkShaderWrappers) {
		delete vkShaderWrapper;
	}

	vkShaderWrappers.clear();
}

std::vector<VkPipelineShaderStageCreateInfo> _VkShaderPipeline::getShaderStages() {
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};
	for (auto vkShaderWrapper : vkShaderWrappers) {
		if (vkShaderWrapper != nullptr) {
			shaderStages.push_back(vkShaderWrapper->getShaderStageInfo());
		}
	}

	return shaderStages;
}

void _VkShaderPipeline::createShader(const char* shaderCode, shaderc_shader_kind shaderType) {
	auto _vkLogger = _VkLogger::Instance();
	if (_pDevice == nullptr) {
		_vkLogger.LogText("_VkShaderPipeline{createShader} => _pDevice is nullptr");
		return;
	}
	// shader init is done in constructor
	VkShaderWrapper* vkShaderWrapper = new VkShaderWrapper(*_pDevice, shaderCode, shaderType);
	vkShaderWrappers.push_back(vkShaderWrapper);
}

VkResult _VkShaderPipeline::create() {
	createShader(vertexShaderCode, shaderc_shader_kind::shaderc_vertex_shader);
	createShader(fragmentShaderCode, shaderc_shader_kind::shaderc_fragment_shader);

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
	vkVertexInputState.vertexBindingDescriptionCount = 0;
	vkVertexInputState.pVertexBindingDescriptions = nullptr; // optional
	vkVertexInputState.vertexAttributeDescriptionCount = 0;
	vkVertexInputState.pVertexAttributeDescriptions = nullptr; // optional

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