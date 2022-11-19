#include "vk_graphics_pipeline.h"

const char* vertexShaderCode = R"(#version 450
layout(location = 0) out vec3 fragColor;
vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);
vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);
void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragColor = colors[gl_VertexIndex];
})";

const char* fragmentShaderCode = R"(#version 450
layout(location = 0) in vec3 fragColor;
layout(location = 0) out vec4 outColor;
void main() {
    outColor = vec4(fragColor, 1.0);
})";

// Public

VkGraphicsPipeline::VkGraphicsPipeline(_VkDevice& _vkDevice) :
	_vkDevice{ _vkDevice } {

}

VkGraphicsPipeline::~VkGraphicsPipeline() {
	for (auto vkShaderWrapper : vkShaderWrappers) {
		delete vkShaderWrapper;
	}
	vkShaderWrappers.clear();
}

std::vector<VkPipelineShaderStageCreateInfo> VkGraphicsPipeline::getShaderStages() {
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};
	for (auto vkShaderWrapper : vkShaderWrappers) {
		if (vkShaderWrapper != nullptr) {
			shaderStages.push_back(vkShaderWrapper->getShaderStageInfo());
		}
	}
	return shaderStages;
}

void VkGraphicsPipeline::addShader(const char* code, shaderc_shader_kind kind) {
	// shader init is done in constructor
	VkShaderWrapper* vkShaderWrapper = new VkShaderWrapper(_vkDevice, code, kind);
	vkShaderWrappers.push_back(vkShaderWrapper);
}

void VkGraphicsPipeline::init() {
	addShader(vertexShaderCode, shaderc_shader_kind::shaderc_vertex_shader);
	addShader(fragmentShaderCode, shaderc_shader_kind::shaderc_fragment_shader);
}

// Static

std::vector<char> VkGraphicsPipeline::readFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}