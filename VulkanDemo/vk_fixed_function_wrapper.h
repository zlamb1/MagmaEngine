#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "vk_swap_chain_wrapper.h"

class VkFixedFunctionWrapper {

	public:

		VkFixedFunctionWrapper(VkDeviceWrapper& _vkDeviceWrapper,
			VkSwapChainWrapper& _vkSwapChainWrapper);
		~VkFixedFunctionWrapper();

		VkPipelineDynamicStateCreateInfo& getDynamicState();
		VkPipelineVertexInputStateCreateInfo& getVertexInputState();
		VkPipelineInputAssemblyStateCreateInfo& getInputAssemblyState();
		VkViewport& getViewport();
		VkRect2D& getRect2D();
		VkPipelineViewportStateCreateInfo& getViewportState();
		VkPipelineRasterizationStateCreateInfo& getRasterizerState();
		VkPipelineMultisampleStateCreateInfo& getMultisamplingState();
		VkPipelineDepthStencilStateCreateInfo& getDepthStencilState();
		VkPipelineColorBlendAttachmentState& getColorBlendAttachmentState();
		VkPipelineColorBlendStateCreateInfo& getColorBlendState();
		VkPipelineLayoutCreateInfo& getPipelineLayoutCreate();
		VkPipelineLayout& getPipelineLayout();

		void init();

	private:

		VkDeviceWrapper& vkDeviceWrapper;
		VkSwapChainWrapper& vkSwapChainWrapper;

		VkPipelineDynamicStateCreateInfo vkDynamicState;
		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineVertexInputStateCreateInfo vkVertexInputState;
		VkPipelineInputAssemblyStateCreateInfo vkInputAssemblyState;
		VkViewport vkViewport; 
		VkRect2D vkRect2D;
		VkPipelineViewportStateCreateInfo vkViewportState;
		VkPipelineRasterizationStateCreateInfo vkRasterizerState;
		VkPipelineMultisampleStateCreateInfo vkMultisamplingState;
		VkPipelineDepthStencilStateCreateInfo vkDepthStencilState;
		VkPipelineColorBlendAttachmentState vkColorBlendAttachmentState;
		VkPipelineColorBlendStateCreateInfo vkColorBlendingState;
		VkPipelineLayoutCreateInfo vkPipelineLayoutInfo;

		VkPipelineLayout vkPipelineLayout;

		void initDynamicState();
		void initVertexState();
		void initInputAssembly();
		void initViewport();
		void initRasterizer();
		void initMultisampling();
		void initDepthStencilTesting();
		void initColorBlending();
		void initPipelineLayout();

};