#include "shader_attributes.h"

namespace Magma {

	const std::vector<ShaderAttributes::VkVertexBinding>& ShaderAttributes::getVertexBindings() {
		return vertexBindings;
	}

	const std::vector<ShaderAttributes::VkVertexAttribute>& ShaderAttributes::getVertexAttributes() {
		return vertexAttributes;
	}

	const std::vector<VkDescriptorSetLayout> ShaderAttributes::getDescriptorSetLayouts() {
		std::vector<VkDescriptorSetLayout> vkDescriptorSetLayouts{};
		for (auto& descriptorSetLayout : descriptorSetLayouts) {
			vkDescriptorSetLayouts.push_back(descriptorSetLayout->getDescriptorSetLayout());
		}
		return vkDescriptorSetLayouts;
	}

	const std::vector<VkDescriptorSet> ShaderAttributes::getDescriptorSets() {
		std::vector<VkDescriptorSet> vkDescriptorSets{};
		for (auto& descriptorSet : this->descriptorSets) {
			auto& sets = descriptorSet->getDescriptorSets();
			vkDescriptorSets.insert(vkDescriptorSets.end(), sets.begin(), sets.end());
		}
		return vkDescriptorSets;
	}

	ShaderAttributes::VkVertexBinding ShaderAttributes::createVertexBinding(
		uint32_t binding, uint32_t stride, VertexInputRate inputRate) {
		ShaderAttributes::VkVertexBinding vertexBinding{};
		vertexBinding.binding = binding;
		vertexBinding.stride = stride;
		vertexBinding.inputRate = (VkVertexInputRate)inputRate;
		vertexBindings.push_back(vertexBinding);
		return vertexBinding;
	}

	ShaderAttributes::VkVertexAttribute ShaderAttributes::createVertexAttribute(
		uint32_t binding, uint32_t location, uint32_t offset, VulkanFormat format) {
		ShaderAttributes::VkVertexAttribute vertexAttribute{};
		vertexAttribute.binding = binding;
		vertexAttribute.location = location;
		vertexAttribute.offset = offset;
		vertexAttribute.format = (VkFormat)format;
		vertexAttributes.push_back(vertexAttribute);
		return vertexAttribute;
	}

	Descriptor ShaderAttributes::createDescriptor(uint32_t pBinding, uint32_t pCount,
		VulkanShaderType pStageFlags) {
		Descriptor vulkanDescriptor{};
		vulkanDescriptor.pBinding = pBinding;
		vulkanDescriptor.pCount = pCount;
		vulkanDescriptor.pStageFlags = pStageFlags;
		vulkanDescriptor.init();
		vulkanDescriptors.push_back(vulkanDescriptor);
		return vulkanDescriptor;
	}

	std::shared_ptr<DescriptorSetLayout> ShaderAttributes::createDescriptorSetLayout() {
		std::shared_ptr<DescriptorSetLayout> descriptorSetLayout =
			std::make_shared<DescriptorSetLayout>();
		descriptorSetLayout->pVulkanDevice = pVulkanDevice;
		for (auto& vulkanDescriptor : vulkanDescriptors) {
			descriptorSetLayout->pDescriptors.push_back(vulkanDescriptor.getLayoutBinding());
		}
		descriptorSetLayout->init();
		descriptorSetLayouts.push_back(descriptorSetLayout);
		return descriptorSetLayout;
	}

	std::shared_ptr<DescriptorSet> ShaderAttributes::createDescriptorSet(VkBuffer& pBuffer,
		uint32_t pMaxSets, VkDeviceSize pSize) {
		std::shared_ptr<DescriptorSet> descriptorSet = std::make_shared<DescriptorSet>();
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

}