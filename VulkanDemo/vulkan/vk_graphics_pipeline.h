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

		VkGraphicsPipeline(VkDeviceWrapper& _vkDeviceWrapper);
		~VkGraphicsPipeline();

		std::vector<VkPipelineShaderStageCreateInfo> getShaderStages();

		void addShader(const char* code, shaderc_shader_kind kind);

		void init();

	private:

		VkDeviceWrapper& vkDeviceWrapper; 

		// store shader wrappers to ensure they don't go out of scope
		std::vector<VkShaderWrapper*> vkShaderWrappers;

		static std::vector<char> readFile(const std::string& filename);

};