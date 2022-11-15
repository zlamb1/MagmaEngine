#pragma once

#include <iostream>
#include <stdexcept>

#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>

#include "vk_device_wrapper.h"

class VkShaderWrapper {

	public:

		VkShaderWrapper(VkDeviceWrapper& _vkDeviceWrapper, const char* shaderCode, 
			shaderc_shader_kind kind);
		~VkShaderWrapper();

		VkPipelineShaderStageCreateInfo& getShaderStageInfo();

	private:
		VkDeviceWrapper& vkDeviceWrapper;
		shaderc::Compiler compiler;

		VkShaderModule vkModule; 
		VkPipelineShaderStageCreateInfo vkShaderStageInfo;

		void initModule(const char* shaderCode,
			shaderc_shader_kind kind);
		void initStage(shaderc_shader_kind kind);
		std::vector<uint32_t> compileShader(const char* data, shaderc_shader_kind kind);

};