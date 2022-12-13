#pragma once

#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>

#include "Magma/Vulkan/Device/vulkan_device.h"

namespace Magma {

	class Shader : public VulkanObject {

	public:
		Shader(std::shared_ptr<VulkanDevice> device);
		~Shader() override;

		VkResult init() override;

		VkShaderModule& getShaderModule();
		VkPipelineShaderStageCreateInfo& getShaderStageCreateInfo();

	public:
		std::shared_ptr<VulkanDevice> m_Device = nullptr;

		const char* m_ShaderCode = nullptr;
		shaderc_shader_kind m_ShaderType = shaderc_vertex_shader;

		shaderc::Compiler m_Compiler{};

	private:
		VkShaderModule m_ShaderModule{};
		VkPipelineShaderStageCreateInfo m_ShaderStageCreateInfo{};

	};

	enum class ShaderType {
		COMPUTE = VK_SHADER_STAGE_COMPUTE_BIT,
		GEOMETRY = VK_SHADER_STAGE_GEOMETRY_BIT,
		VERTEX = VK_SHADER_STAGE_VERTEX_BIT,
		FRAGMENT = VK_SHADER_STAGE_FRAGMENT_BIT
	};

}