#include "shader.h"

namespace ShaderUtility {
	static std::vector<uint32_t> compileShader(const shaderc::Compiler& compiler,
		const char* shaderCode, shaderc_shader_kind shaderType) {
		const shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(shaderCode,
			shaderType, "");
		if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
			std::cerr << result.GetErrorMessage() << std::endl;
		}
		return { result.cbegin(), result.cend() };
	}
}

namespace Magma {

	Shader::Shader(std::shared_ptr<VulkanDevice> device) :
		m_Device{ std::move(device) } {}

	Shader::~Shader() {
		if (m_Device != nullptr) {
			vkDestroyShaderModule(m_Device->getDevice(), m_ShaderModule, pAllocator);
		}
	}

	VkResult Shader::init() {
		if (m_Device == nullptr) {
			Z_LOG_TXT("VulkanShader::init", "pDevice is nullptr");
			return VK_ERROR_INITIALIZATION_FAILED;
		}

		if (m_ShaderCode == nullptr) {
			Z_LOG_TXT("VulkanShader::init", "pShaderCode is nullptr");
			return VK_ERROR_INITIALIZATION_FAILED;
		}

		const std::vector<uint32_t> shaderResult = ShaderUtility::compileShader(m_Compiler,
			m_ShaderCode, m_ShaderType);

		VkShaderModuleCreateInfo shaderModuleCreateInfo{};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.codeSize = shaderResult.size() * sizeof(uint32_t);
		shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaderResult.data());

		auto createShaderModuleResult = vkCreateShaderModule(m_Device->getDevice(),
			&shaderModuleCreateInfo, pAllocator, &m_ShaderModule);
		Z_LOG_OBJ("VulkanShader::init::vkCreateShaderModule", createShaderModuleResult);

		if (createShaderModuleResult != VK_SUCCESS) {
			return createShaderModuleResult;
		}

		// VkPipelineShaderStageCreateInfo init
		m_ShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		switch (m_ShaderType) {
			case 0:
				m_ShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
				break;
			case 1:
				m_ShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
				break;
			default:
				m_ShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		}

		m_ShaderStageCreateInfo.module = m_ShaderModule;
		// shader entry point
		m_ShaderStageCreateInfo.pName = "main";
		// allows you to set shader constants
		m_ShaderStageCreateInfo.pSpecializationInfo = nullptr;

		return VK_SUCCESS;
	}

	VkShaderModule& Shader::getShaderModule() {
		return m_ShaderModule;
	}

	VkPipelineShaderStageCreateInfo& Shader::getShaderStageCreateInfo() {
		return m_ShaderStageCreateInfo;
	}

}