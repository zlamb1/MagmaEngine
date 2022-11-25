#include "vulkan_pipeline_c.h"

// VulkanShaderPipeline 

std::vector<VkPipelineShaderStageCreateInfo> VulkanShaderPipeline::getShaderStages() {
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};
	for (auto vulkanShaderHandle : vulkanShaderHandles) {
		if (vulkanShaderHandle != nullptr) {
			shaderStages.push_back(vulkanShaderHandle->getShaderStageCreateInfo());
		}
	}

	return shaderStages;
}

void VulkanShaderPipeline::addShader(VulkanShader* _vkShaderHandle) {
	vulkanShaderHandles.push_back(_vkShaderHandle);
}

VkResult VulkanShaderPipeline::init() {
	return VK_SUCCESS;
}

// VulkanFixedFunctionState

VulkanFixedFunctionState::~VulkanFixedFunctionState() {
	if (pDevice != nullptr) {
		vkDestroyPipelineLayout(pDevice->getDevice(), vkPipelineLayout, nullptr);
	}
}

VkResult VulkanFixedFunctionState::init() {
	if (pDevice == nullptr) {
		VulkanLogger::instance().enqueueText("VulkanFixedFunctionState::init", "pDevice is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	if (pSwapchain == nullptr) {
		VulkanLogger::instance().enqueueText("VulkanFixedFunctionState::init", "pSwapchain is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	vkDynamiCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	vkDynamiCreateInfo.dynamicStateCount = static_cast<uint32_t>(vkDynamicStates.size());
	vkDynamiCreateInfo.pDynamicStates = vkDynamicStates.data();

	vkVertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vkVertexInputCreateInfo.vertexBindingDescriptionCount =
		static_cast<uint32_t>(pVertexBindingDescriptions.size());
	vkVertexInputCreateInfo.pVertexBindingDescriptions = pVertexBindingDescriptions.data();
	vkVertexInputCreateInfo.vertexAttributeDescriptionCount =
		static_cast<uint32_t>(pVertexAttributeDescriptions.size());
	vkVertexInputCreateInfo.pVertexAttributeDescriptions = pVertexAttributeDescriptions.data();

	vkInputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	vkInputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	vkInputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

	auto vkSwapchainExtent = pSwapchain->getSwapchainExtent();
	vkViewport.x = 0.0f;
	vkViewport.y = 0.0f;
	vkViewport.width = (float)vkSwapchainExtent.width;
	vkViewport.height = (float)vkSwapchainExtent.height;
	vkViewport.minDepth = 0.0f;
	vkViewport.maxDepth = 1.0f;

	vkScissor.offset = { 0, 0 };
	vkScissor.extent = vkSwapchainExtent;

	vkViewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	vkViewportCreateInfo.viewportCount = 1;
	vkViewportCreateInfo.pViewports = &vkViewport;
	vkViewportCreateInfo.scissorCount = 1;
	vkViewportCreateInfo.pScissors = &vkScissor;

	vkRasterizationCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	vkRasterizationCreateInfo.depthClampEnable = VK_FALSE;
	vkRasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	vkRasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	vkRasterizationCreateInfo.lineWidth = 1.0f;
	vkRasterizationCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	vkRasterizationCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	vkRasterizationCreateInfo.depthBiasEnable = VK_FALSE;
	vkRasterizationCreateInfo.depthBiasConstantFactor = 0.0f; // optional
	vkRasterizationCreateInfo.depthBiasClamp = 0.0f; // optional
	vkRasterizationCreateInfo.depthBiasSlopeFactor = 0.0f; // optional

	vkMultisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	vkMultisampleCreateInfo.sampleShadingEnable = VK_FALSE;
	vkMultisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	vkMultisampleCreateInfo.minSampleShading = 1.0f; // optional
	vkMultisampleCreateInfo.pSampleMask = nullptr; // optional
	vkMultisampleCreateInfo.alphaToCoverageEnable = VK_FALSE; // optional
	vkMultisampleCreateInfo.alphaToOneEnable = VK_FALSE; // optional

	vkColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT 
		| VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	vkColorBlendAttachmentState.blendEnable = VK_FALSE;
	vkColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // optional
	vkColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // optional
	vkColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD; // optional
	vkColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // optional
	vkColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // optional
	vkColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD; // optional

	vkColorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	vkColorBlendCreateInfo.logicOpEnable = VK_FALSE;
	vkColorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY; // optional
	vkColorBlendCreateInfo.attachmentCount = 1;
	vkColorBlendCreateInfo.pAttachments = &vkColorBlendAttachmentState;
	vkColorBlendCreateInfo.blendConstants[0] = 0.0f; // optional
	vkColorBlendCreateInfo.blendConstants[1] = 0.0f; // optional
	vkColorBlendCreateInfo.blendConstants[2] = 0.0f; // optional
	vkColorBlendCreateInfo.blendConstants[3] = 0.0f; // optional

	vkPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	vkPipelineLayoutCreateInfo.setLayoutCount = 0; // optional
	vkPipelineLayoutCreateInfo.pSetLayouts = nullptr; // optional
	vkPipelineLayoutCreateInfo.pushConstantRangeCount = 0; // optional
	vkPipelineLayoutCreateInfo.pPushConstantRanges = nullptr; // optional

	auto createPipelineLayoutResult = vkCreatePipelineLayout(pDevice->getDevice(),
		&vkPipelineLayoutCreateInfo, nullptr, &vkPipelineLayout);
	VulkanLogger::instance().enqueueObject("VulkanFixedFunctionState::init::vkCreatePipelineLayoutResult", 
		createPipelineLayoutResult);

	return createPipelineLayoutResult;
}

VkPipelineDynamicStateCreateInfo& VulkanFixedFunctionState::getDynamicCreateInfo() {
	return vkDynamiCreateInfo;
}

VkPipelineVertexInputStateCreateInfo& VulkanFixedFunctionState::getVertexInputCreateInfo() {
	return vkVertexInputCreateInfo;
}

VkPipelineInputAssemblyStateCreateInfo& VulkanFixedFunctionState::getInputAssemblyCreateInfo() {
	return vkInputAssemblyCreateInfo;
}

VkViewport& VulkanFixedFunctionState::getViewport() {
	return vkViewport;
}

VkRect2D& VulkanFixedFunctionState::getScissor() {
	return vkScissor;
}

VkPipelineViewportStateCreateInfo& VulkanFixedFunctionState::getViewportCreateInfo() {
	return vkViewportCreateInfo;
}

VkPipelineRasterizationStateCreateInfo& VulkanFixedFunctionState::getRasterizationCreateInfo() {
	return vkRasterizationCreateInfo;
}

VkPipelineMultisampleStateCreateInfo& VulkanFixedFunctionState::getMultisampleCreateInfo() {
	return vkMultisampleCreateInfo;
}

VkPipelineDepthStencilStateCreateInfo& VulkanFixedFunctionState::getDepthStencilCreateInfo() {
	return vkDepthStencilCreateInfo;
}

VkPipelineColorBlendAttachmentState& VulkanFixedFunctionState::getColorBlendAttachmentState() {
	return vkColorBlendAttachmentState;
}

VkPipelineColorBlendStateCreateInfo& VulkanFixedFunctionState::getColorBlendCreateInfo() {
	return vkColorBlendCreateInfo;
}

VkPipelineLayoutCreateInfo& VulkanFixedFunctionState::getPipelineLayoutCreateInfo() {
	return vkPipelineLayoutCreateInfo;
}

VkPipelineLayout& VulkanFixedFunctionState::getPipelineLayout() {
	return vkPipelineLayout;
}

// VulkanRenderPass

VulkanRenderPass::~VulkanRenderPass() {
	if (pDevice != nullptr) {
		vkDestroyRenderPass(pDevice->getDevice(), vkRenderPass, nullptr);
	}
}

VkResult VulkanRenderPass::init() {
	if (pDevice == nullptr) {
		VulkanLogger::instance().enqueueText("VulkanRenderPass::init", "pDevice is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	if (pSwapchain == nullptr) {
		VulkanLogger::instance().enqueueText("VulkanRenderPass::init", "pSwapchain is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = pSwapchain->getSwapchainImageFormat();
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

	auto createRenderPassResult = vkCreateRenderPass(pDevice->getDevice(), &renderPassInfo,
		nullptr, &vkRenderPass);
	VulkanLogger::instance().enqueueObject("VulkanRenderPass::init::vkCreateRenderPass", 
		createRenderPassResult);
	
	return createRenderPassResult;
}

VkRenderPass& VulkanRenderPass::getRenderPass() {
	return vkRenderPass;
}