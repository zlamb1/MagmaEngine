#pragma once

#include <fstream>
#include <iostream>

#include <shaderc/shaderc.hpp>
#include <shaderc/shaderc.h>
#include <vulkan/vulkan.h>

#include "vk_device_wrapper.h"
#include "vk_shader_wrapper.h"

class VkGraphicsPipeline {

	public:

		VkGraphicsPipeline(_VkDevice& _vkDevice);
		~VkGraphicsPipeline();

		std::vector<VkPipelineShaderStageCreateInfo> getShaderStages();

		void addShader(const char* code, shaderc_shader_kind kind);

		void init();

	private:

		_VkDevice& _vkDevice;

		// store shader wrappers to ensure they don't go out of scope
		std::vector<VkShaderWrapper*> vkShaderWrappers;

		static std::vector<char> readFile(const std::string& filename);

};