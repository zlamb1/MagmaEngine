#pragma once

#include <iostream>
#include <stdexcept>

#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>

#include "Vulktrix/Device/vulkan_device.h"

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

enum class VulkanShaderType {
	COMPUTE = VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT,
	GEOMETRY = VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT,
	VERTEX = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT,
	FRAGMENT = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT
};