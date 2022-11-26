#pragma once

#include <iostream>
#include <stdexcept>

#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>

#include "vulkan_device.h"

class VulkanShader : public VulkanObject {

public:
	VulkanShader(std::shared_ptr<VulkanDevice> pVulkanDevice);
	~VulkanShader() override;

	std::shared_ptr<VulkanDevice> pVulkanDevice = nullptr;

	const char* pShaderCode = nullptr;
	shaderc_shader_kind pShaderType = shaderc_shader_kind::shaderc_vertex_shader; 

	shaderc::Compiler pCompiler{};

	VkResult init() override;

	VkShaderModule& getShaderModule();
	VkPipelineShaderStageCreateInfo& getShaderStageCreateInfo();

private:
	VkShaderModule vkShaderModule{};
	VkPipelineShaderStageCreateInfo vkShaderStageCreateInfo{};

};