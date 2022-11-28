#include "shader_attributes.h"

const std::vector<ShaderAttributes::VkVertexBinding>& ShaderAttributes::getVertexBindings() {
	return vertexBindings;
}

const std::vector<ShaderAttributes::VkVertexAttribute>& ShaderAttributes::getVertexAttributes() {
	return vertexAttributes;
}

const std::vector<VkDescriptorSetLayout> ShaderAttributes::getDescriptorSetLayouts() {
	std::vector<VkDescriptorSetLayout> vkDescriptorSetLayouts{};
	for (auto vkDescriptorSetLayout : descriptorSetLayouts) {
		vkDescriptorSetLayouts.push_back(vkDescriptorSetLayout->getDescriptorSetLayout());
	}
	return vkDescriptorSetLayouts;
}

const std::vector<VkDescriptorSet> ShaderAttributes::getDescriptorSets() {
	std::vector<VkDescriptorSet> vkDescriptorSets{};
	for (auto descriptorSet : descriptorSets) {
		for (auto vkDescriptorSet : descriptorSet->getDescriptorSets()) {
			vkDescriptorSets.push_back(vkDescriptorSet);
		}
	}
	return vkDescriptorSets;
}

ShaderAttributes::VkVertexBinding ShaderAttributes::createVertexBinding(
	uint32_t binding, uint32_t stride, VertexInputRate inputRate) {
	ShaderAttributes::VkVertexBinding vertexBinding{};
	vertexBinding.binding = binding;
	vertexBinding.stride = stride;
	vertexBinding.inputRate = (VkVertexInputRate) inputRate;
	vertexBindings.push_back(vertexBinding);
	return vertexBinding;
}

ShaderAttributes::VkVertexAttribute ShaderAttributes::createVertexAttribute(
	uint32_t binding, uint32_t location, uint32_t offset, VulkanFormat format) {
	ShaderAttributes::VkVertexAttribute vertexAttribute{};
	vertexAttribute.binding = binding;
	vertexAttribute.location = location;
	vertexAttribute.offset = offset;
	vertexAttribute.format = (VkFormat) format;
	vertexAttributes.push_back(vertexAttribute);
	return vertexAttribute;
}

VulkanDescriptor ShaderAttributes::createDescriptor(uint32_t pBinding, uint32_t pCount,
	VulkanShaderType pStageFlags) {
	VulkanDescriptor vulkanDescriptor{};
	vulkanDescriptor.pBinding = pBinding;
	vulkanDescriptor.pCount = pCount;
	vulkanDescriptor.pStageFlags = pStageFlags;
	vulkanDescriptor.init();
	vulkanDescriptors.push_back(vulkanDescriptor);
	return vulkanDescriptor;
}

std::shared_ptr<VulkanDescriptorSetLayout> ShaderAttributes::createDescriptorSetLayout() {
	std::shared_ptr<VulkanDescriptorSetLayout> descriptorSetLayout =
		std::make_shared<VulkanDescriptorSetLayout>();
	descriptorSetLayout->pVulkanDevice = pVulkanDevice;
	for (auto& vulkanDescriptor : vulkanDescriptors) {
		descriptorSetLayout->pDescriptors.push_back(vulkanDescriptor.getLayoutBinding());
	}
	descriptorSetLayout->init();
	descriptorSetLayouts.push_back(descriptorSetLayout);
	return descriptorSetLayout;
}

std::shared_ptr<VulkanDescriptorSet> ShaderAttributes::createDescriptorSet(VkBuffer& pBuffer,
	uint32_t pMaxSets, VkDeviceSize pSize) {
	std::shared_ptr<VulkanDescriptorSet> descriptorSet = std::make_shared<VulkanDescriptorSet>();
	descriptorSet->pVulkanDevice = pVulkanDevice;

	auto descriptorSetLayouts = getDescriptorSetLayouts();
	for (auto descriptorSetLayout : descriptorSetLayouts) {
		descriptorSet->pDescriptorSetLayouts.push_back(descriptorSetLayout);
	}

	descriptorSet->pMaxSets = pMaxSets;
	descriptorSet->pBuffer = pBuffer;
	descriptorSet->pSize = pSize;
	descriptorSet->init();
	descriptorSets.push_back(descriptorSet);
	return descriptorSet;
}

void ShaderAttributes::clearVertexBindings() {
	vertexBindings.clear();
}

void ShaderAttributes::clearVertexAttributes() {
	vertexAttributes.clear();
}

void ShaderAttributes::clearDescriptors() {
	vulkanDescriptors.clear();
}

void ShaderAttributes::clearDescriptorSetLayouts() {
	descriptorSetLayouts.clear();
}

void ShaderAttributes::clearDescriptorSets() {
	descriptorSets.clear();
}