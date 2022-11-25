#include "vulkan_shader.h"

namespace shader_utility {
	static std::vector<uint32_t> compileShader(shaderc::Compiler& compiler,
		const char* shaderCode, shaderc_shader_kind shaderType) {
		shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(shaderCode,
			shaderType, "");
		if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
			std::cerr << result.GetErrorMessage() << std::endl;
		}
		return { result.cbegin(), result.cend() };
	}
}

// VulkanShader

VulkanShader::~VulkanShader() {
	if (pDevice != nullptr) {
		vkDestroyShaderModule(*pDevice, vkShaderModule, nullptr);
	}
}

VkResult VulkanShader::init() {
	if (pDevice == nullptr) {
		VulkanLogger::instance().enqueueText("VulkanShader::init", "pDevice is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	if (pShaderCode == nullptr) {
		VulkanLogger::instance().enqueueText("VulkanShader::init", "pShaderCode is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	// VkShaderModule init
	std::vector<uint32_t> vkShaderResult = shader_utility::compileShader(pCompiler,
		pShaderCode, pShaderType);

	VkShaderModuleCreateInfo vkShaderModuleCreateInfo{};
	vkShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vkShaderModuleCreateInfo.codeSize = vkShaderResult.size() * sizeof(uint32_t);
	vkShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(vkShaderResult.data());

	auto createShaderModuleResult = vkCreateShaderModule(*pDevice, &vkShaderModuleCreateInfo,
		nullptr, &vkShaderModule);
	VulkanLogger::instance().enqueueObject("VulkanShader::init::vkCreateShaderModule", 
		createShaderModuleResult);

	if (createShaderModuleResult != VK_SUCCESS) {
		return createShaderModuleResult;
	}

	// VkPipelineShaderStageCreateInfo init
	vkShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	switch (pShaderType) {
		case 0:
			vkShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			break;
		case 1:
			vkShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			break;
		default:
			vkShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	}

	vkShaderStageCreateInfo.module = vkShaderModule;
	// shader entry point
	vkShaderStageCreateInfo.pName = "main";
	// allows you to set shader constants
	vkShaderStageCreateInfo.pSpecializationInfo = nullptr;

	return VK_SUCCESS;
}

VkShaderModule& VulkanShader::getShaderModule() {
	return vkShaderModule;
}

VkPipelineShaderStageCreateInfo& VulkanShader::getShaderStageCreateInfo() {
	return vkShaderStageCreateInfo;
}

