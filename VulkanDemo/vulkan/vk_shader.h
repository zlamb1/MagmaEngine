#pragma once

#include <iostream>
#include <stdexcept>

#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>

#include "vk_device_wrapper.h"

struct _VkShader : VulkanWrapper {

	_VkShader();
	~_VkShader();

	shaderc::Compiler compiler{};

	VkDevice* pDevice = nullptr;
	const char* pShaderCode = nullptr;
	shaderc_shader_kind pShaderType = shaderc_shader_kind::shaderc_vertex_shader;

	VkShaderModule vkShaderModule{};
	VkPipelineShaderStageCreateInfo vkShaderStageInfo{};

	VkResult create();

};