#pragma once

#include <iostream>
#include <stdexcept>

#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>

#include "vk_device_wrapper.h"

class VkShaderWrapper {

	public:

		VkShaderWrapper(_VkDevice& _vkDevice, const char* shaderCode,
			shaderc_shader_kind kind);
		~VkShaderWrapper();

		VkPipelineShaderStageCreateInfo& getShaderStageInfo();

	private:
		_VkDevice& _vkDevice;
		shaderc::Compiler compiler{};

		VkShaderModule vkModule; 
		VkPipelineShaderStageCreateInfo vkShaderStageInfo;

		void initModule(const char* shaderCode,
			shaderc_shader_kind kind);
		void initStage(shaderc_shader_kind kind);
		std::vector<uint32_t> compileShader(const char* data, shaderc_shader_kind kind);

};