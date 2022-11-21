#pragma once

#include <vulkan/vulkan.h>

#include "vk_device_wrapper.h"
#include "vk_pipeline_components.h"
#include "vk_framebuffer.h"
#include "vk_cmd_wrapper.h"
#include "vk_attribute_description.h"

struct _VkPipeline : VulkanWrapper {

	_VkPipeline();
	~_VkPipeline();

	_VkLogger& _vkLogger; 

	_VkDevice* _pDevice = nullptr;
	_VkSwapchain* _pSwapchain = nullptr;

	std::vector<VkVertexInputBindingDescription> pBindingDescriptions{};
	std::vector<_VkAttributeDescription> pAttributeDescriptions{};

	_VkShaderPipeline _vkShaderPipeline;
	std::unique_ptr<_VkFixedFunctionState> _vkFixedFunctionState;
	std::unique_ptr<_VkRenderPass> _vkRenderPass;
	_VkFramebuffer* _vkFramebuffer = nullptr;

	VkPipeline vkPipeline{};

	VkResult create();
	VkResult initFramebuffers();

	void addShader(_VkShaderInfo _vkShaderInfo);
	void deleteFramebuffers();

	void onNewFrame(_VkCmdBuffer& vkCmdBuffer, uint32_t imageIndex);

};