#include "vk_fixed_function_wrapper.h"

// Public

VkFixedFunctionWrapper::VkFixedFunctionWrapper(
    VkDeviceWrapper& _vkDeviceWrapper,
    VkSwapChainWrapper& _vkSwapChainWrapper) :
    vkDeviceWrapper{ _vkDeviceWrapper },
    vkSwapChainWrapper{ _vkSwapChainWrapper }
{

}

VkFixedFunctionWrapper::~VkFixedFunctionWrapper() {
    vkDestroyPipelineLayout(vkDeviceWrapper.getLogicalDevice(), vkPipelineLayout, nullptr);
}

VkPipelineDynamicStateCreateInfo& VkFixedFunctionWrapper::getDynamicState() {
    return vkDynamicState;
}

VkPipelineVertexInputStateCreateInfo& VkFixedFunctionWrapper::getVertexInputState() {
    return vkVertexInputState;
}

VkPipelineInputAssemblyStateCreateInfo& VkFixedFunctionWrapper::getInputAssemblyState() {
    return vkInputAssemblyState;
}

VkViewport& VkFixedFunctionWrapper::getViewport() {
    return vkViewport;
}

VkRect2D& VkFixedFunctionWrapper::getRect2D() {
    return vkRect2D;
}

VkPipelineViewportStateCreateInfo& VkFixedFunctionWrapper::getViewportState() {
    return vkViewportState;
}

VkPipelineRasterizationStateCreateInfo& VkFixedFunctionWrapper::getRasterizerState() {
    return vkRasterizerState;
}

VkPipelineMultisampleStateCreateInfo& VkFixedFunctionWrapper::getMultisamplingState() {
    return vkMultisamplingState;
}

VkPipelineDepthStencilStateCreateInfo& VkFixedFunctionWrapper::getDepthStencilState() {
    return vkDepthStencilState;
}

VkPipelineColorBlendAttachmentState& VkFixedFunctionWrapper::getColorBlendAttachmentState() {
    return vkColorBlendAttachmentState;
}

VkPipelineColorBlendStateCreateInfo& VkFixedFunctionWrapper::getColorBlendState() {
    return vkColorBlendingState;
}

VkPipelineLayoutCreateInfo& VkFixedFunctionWrapper::getPipelineLayoutCreate() {
    return vkPipelineLayoutInfo;
}

VkPipelineLayout& VkFixedFunctionWrapper::getPipelineLayout() {
    return vkPipelineLayout;
}

void VkFixedFunctionWrapper::init() {
    initDynamicState();
    initVertexState();
    initInputAssembly();
    initViewport();
    initRasterizer();
    initMultisampling();
    initDepthStencilTesting();
    initColorBlending();
    initPipelineLayout();
}

// Private

void VkFixedFunctionWrapper::initDynamicState() {
    vkDynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    vkDynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    vkDynamicState.pDynamicStates = dynamicStates.data();
}

void VkFixedFunctionWrapper::initVertexState() {
    vkVertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vkVertexInputState.vertexBindingDescriptionCount = 0;
    vkVertexInputState.pVertexBindingDescriptions = nullptr; // optional
    vkVertexInputState.vertexAttributeDescriptionCount = 0;
    vkVertexInputState.pVertexAttributeDescriptions = nullptr; // optional
}

void VkFixedFunctionWrapper::initInputAssembly() {
    vkInputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    vkInputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    vkInputAssemblyState.primitiveRestartEnable = VK_FALSE;
}

void VkFixedFunctionWrapper::initViewport() {
    auto swapChainExtent = vkSwapChainWrapper.getSwapChainExtent();

    vkViewport.x = 0.0f;
    vkViewport.y = 0.0f;
    vkViewport.width = (float) swapChainExtent.width;
    vkViewport.height = (float) swapChainExtent.height;
    vkViewport.minDepth = 0.0f;
    vkViewport.maxDepth = 1.0f;

    vkRect2D.offset = { 0, 0 };
    vkRect2D.extent = swapChainExtent;


    vkViewportState.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vkViewportState.viewportCount = 1;
    vkViewportState.pViewports = &vkViewport;
    vkViewportState.scissorCount = 1;
    vkViewportState.pScissors = &vkRect2D;
}

void VkFixedFunctionWrapper::initRasterizer() {
    vkRasterizerState.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
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
}

void VkFixedFunctionWrapper::initMultisampling() {
    vkMultisamplingState.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    vkMultisamplingState.sampleShadingEnable = VK_FALSE;
    vkMultisamplingState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    vkMultisamplingState.minSampleShading = 1.0f; // optional
    vkMultisamplingState.pSampleMask = nullptr; // optional
    vkMultisamplingState.alphaToCoverageEnable = VK_FALSE; // optional
    vkMultisamplingState.alphaToOneEnable = VK_FALSE; // optional
}

void VkFixedFunctionWrapper::initDepthStencilTesting() {
    // TODO
}

void VkFixedFunctionWrapper::initColorBlending() {
    vkColorBlendAttachmentState.colorWriteMask = 
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT 
        | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    vkColorBlendAttachmentState.blendEnable = VK_FALSE;
    vkColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // optional
    vkColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // optional
    vkColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD; // optional
    vkColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // optional
    vkColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // optional
    vkColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD; // optional

    vkColorBlendingState.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    vkColorBlendingState.logicOpEnable = VK_FALSE;
    vkColorBlendingState.logicOp = VK_LOGIC_OP_COPY; // optional
    vkColorBlendingState.attachmentCount = 1;
    vkColorBlendingState.pAttachments = &vkColorBlendAttachmentState;
    vkColorBlendingState.blendConstants[0] = 0.0f; // optional
    vkColorBlendingState.blendConstants[1] = 0.0f; // optional
    vkColorBlendingState.blendConstants[2] = 0.0f; // optional
    vkColorBlendingState.blendConstants[3] = 0.0f; // optional
}

void VkFixedFunctionWrapper::initPipelineLayout() {
    vkPipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    vkPipelineLayoutInfo.setLayoutCount = 0; // optional
    vkPipelineLayoutInfo.pSetLayouts = nullptr; // optional
    vkPipelineLayoutInfo.pushConstantRangeCount = 0; // optional
    vkPipelineLayoutInfo.pPushConstantRanges = nullptr; // optional

    if (vkCreatePipelineLayout(vkDeviceWrapper.getLogicalDevice(), &vkPipelineLayoutInfo,
        nullptr, &vkPipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}