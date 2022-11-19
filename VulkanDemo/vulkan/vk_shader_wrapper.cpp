#include "vk_shader_wrapper.h"

// Public

VkShaderWrapper::VkShaderWrapper(VkDeviceWrapper& _vkDeviceWrapper, 
	const char* shaderCode, shaderc_shader_kind kind) :
	vkDeviceWrapper{ _vkDeviceWrapper }
{
	initModule(shaderCode, kind);
	initStage(kind);

	std::cout << "Compiled shader." << std::endl << std::endl;
}

VkShaderWrapper::~VkShaderWrapper() {
	vkDestroyShaderModule(vkDeviceWrapper.vkDevice, vkModule, nullptr);
}

VkPipelineShaderStageCreateInfo& VkShaderWrapper::getShaderStageInfo() {
	return vkShaderStageInfo;
}

// Private

void VkShaderWrapper::initModule(const char* shaderCode,
	shaderc_shader_kind kind) {
	std::vector<uint32_t> result = compileShader(shaderCode, kind);

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = result.size() * sizeof(uint32_t);
	createInfo.pCode = reinterpret_cast<const uint32_t*>(result.data());

	if (vkCreateShaderModule(vkDeviceWrapper.vkDevice,
		&createInfo, nullptr, &vkModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}
	else {
		std::cout << "Created module." << std::endl;
	}
}

void VkShaderWrapper::initStage(shaderc_shader_kind kind) {
	vkShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

	switch (kind) {
		case 0:
			vkShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			break;
		case 1:
			vkShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			break;
		default:
			vkShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	}
	
	vkShaderStageInfo.module = vkModule;
	vkShaderStageInfo.pName = "main";
	// allows you to set shader constants
	vkShaderStageInfo.pSpecializationInfo = nullptr;
}

std::vector<uint32_t> VkShaderWrapper::compileShader(const char* data, 
	shaderc_shader_kind kind) 
{
	shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(data, kind, "");
	if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
		std::cerr << result.GetErrorMessage() << std::endl;
	}
	return { result.cbegin(), result.cend() };
}