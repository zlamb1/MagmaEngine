#pragma once

#include <iostream>
#include <stdexcept>

#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>

#include "Magma/Vulkan/Device/vulkan_device.h"

namespace Magma {

	class MagmaShader : public VulkanObject {

	public:
		MagmaShader(std::shared_ptr<VulkanDevice> pVulkanDevice);
		~MagmaShader() override;

		VkResult init() override;

		VkShaderModule& getShaderModule();
		VkPipelineShaderStageCreateInfo& getShaderStageCreateInfo();

	public:
		std::shared_ptr<VulkanDevice> pVulkanDevice = nullptr;

		const char* pShaderCode = nullptr;
		shaderc_shader_kind pShaderType = shaderc_shader_kind::shaderc_vertex_shader;

		shaderc::Compiler pCompiler{};

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

}