#include "vk_shader.h"

namespace _VkShaderUtility {

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

// _VkShader Implementation

_VkShader::_VkShader() {

}

_VkShader::~_VkShader() {
	if (pDevice != nullptr) {
		vkDestroyShaderModule(*pDevice, vkShaderModule, nullptr);
	}
}

VkResult _VkShader::create() {
	auto _vkLogger = _VkLogger::Instance();

	if (pDevice == nullptr) {
		_vkLogger.LogText("_VkShader => pDevice is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	if (pShaderCode == nullptr) {
		_vkLogger.LogText("_VkShader => pShaderCode is nullptr");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	// VkShaderModule init
	std::vector<uint32_t> vkShaderResult = _VkShaderUtility::compileShader(compiler, 
		pShaderCode, pShaderType);

	VkShaderModuleCreateInfo vkShaderModuleCreateInfo{};
	vkShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vkShaderModuleCreateInfo.codeSize = vkShaderResult.size() * sizeof(uint32_t);
	vkShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(vkShaderResult.data());

	auto vkCreateShaderModuleResult = vkCreateShaderModule(*pDevice, &vkShaderModuleCreateInfo,
		nullptr, &vkShaderModule);
	_vkLogger.LogResult("vkCreateShaderModule =>", vkCreateShaderModuleResult);

	if (vkCreateShaderModuleResult != VK_SUCCESS) {
		return vkCreateShaderModuleResult;
	}

	// VkPipelineShaderStageCreateInfo init
	vkShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	switch (pShaderType) {
		case 0:
			vkShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			break;
		case 1:
			vkShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			break;
		default:
			vkShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	}
	vkShaderStageInfo.module = vkShaderModule;
	// shader entry point
	vkShaderStageInfo.pName = "main";
	// allows you to set shader constants
	vkShaderStageInfo.pSpecializationInfo = nullptr;

	return VK_SUCCESS;
}