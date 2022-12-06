#include "shader_attributes.h"

namespace Magma {

	VkVertexBinding VulkanVertexBinding::getVertexBinding() {
		VkVertexBinding vertexBinding{};
		vertexBinding.binding = binding;
		vertexBinding.stride = stride;
		vertexBinding.inputRate = Convert::convertVertexInputRate(inputRate); 

		return vertexBinding;
	}

	VkVertexAttribute VulkanVertexAttribute::getVertexAttribute() {
		VkVertexAttribute vertexAttribute{};
		vertexAttribute.binding = binding;
		vertexAttribute.location = location;
		vertexAttribute.offset = offset;
		vertexAttribute.format = Convert::convertFormat(format);

		return vertexAttribute;
	}

	const std::vector<VulkanVertexBinding>& VulkanShaderAttributes::getVertexBindings() {
		return vertexBindings;
	}

	const std::vector<VulkanVertexAttribute>& VulkanShaderAttributes::getVertexAttributes() {
		return vertexAttributes;
	}

	const std::vector<VkDescriptorSetLayout> VulkanShaderAttributes::getDescriptorSetLayouts() {
		std::vector<VkDescriptorSetLayout> vkDescriptorSetLayouts{};
		for (auto& descriptorSetLayout : descriptorSetLayouts) {
			vkDescriptorSetLayouts.push_back(descriptorSetLayout->getDescriptorSetLayout());
		}

		return vkDescriptorSetLayouts;
	}

	const std::vector<VkDescriptorSet> VulkanShaderAttributes::getDescriptorSets() {
		std::vector<VkDescriptorSet> vkDescriptorSets{};
		for (auto& descriptorSet : this->descriptorSets) {
			auto& sets = descriptorSet->getDescriptorSets();
			vkDescriptorSets.insert(vkDescriptorSets.end(), sets.begin(), sets.end());
		}

		return vkDescriptorSets;
	}

	VertexBinding VulkanShaderAttributes::createVertexBinding(
		uint32_t binding, uint32_t stride, VertexInputRate inputRate) {
		VulkanVertexBinding vertexBinding{};
		vertexBinding.binding = binding;
		vertexBinding.stride = stride;
		vertexBinding.inputRate = inputRate;

		vertexBindings.push_back(vertexBinding);

		return vertexBinding;
	}

	VertexAttribute VulkanShaderAttributes::createVertexAttribute(
		uint32_t binding, uint32_t location, uint32_t offset, DataFormat format) {
		VulkanVertexAttribute vertexAttribute{};
		vertexAttribute.binding = binding;
		vertexAttribute.location = location;
		vertexAttribute.offset = offset;
		vertexAttribute.format = format;

		vertexAttributes.push_back(vertexAttribute);

		return vertexAttribute;
	}

	Descriptor VulkanShaderAttributes::createDescriptor(Buffer& pBuffer, 
		uint32_t pBinding, 
		uint64_t pSize,
		uint32_t pCount,
		VulkanShaderType pStageFlags) {
		Descriptor descriptor{};
		descriptor.pBinding = pBinding;
		descriptor.pCount = pCount;
		descriptor.pStageFlags = pStageFlags;

		auto& buffer = (VmaBuffer&)pBuffer;
		descriptor.pBuffer = buffer.getBuffer();
		descriptor.pSize = pSize;

		descriptor.init();

		descriptors.push_back(descriptor);

		return descriptor;
	}

	std::shared_ptr<DescriptorSetLayout> VulkanShaderAttributes::createDescriptorSetLayout() {
		std::shared_ptr<DescriptorSetLayout> descriptorSetLayout =
			std::make_shared<DescriptorSetLayout>();
		descriptorSetLayout->pVulkanDevice = pVulkanDevice;
		for (auto& descriptor : descriptors) {
			descriptorSetLayout->pDescriptors.push_back(descriptor);
		}
		descriptorSetLayout->init();
		descriptorSetLayouts.push_back(descriptorSetLayout);

		return descriptorSetLayout;
	}

	std::shared_ptr<DescriptorSet> VulkanShaderAttributes::createDescriptorSet(uint32_t pMaxSets) {
		std::shared_ptr<DescriptorSet> descriptorSet = std::make_shared<DescriptorSet>();
		descriptorSet->pVulkanDevice = pVulkanDevice;

		for (auto descriptorSetLayout : descriptorSetLayouts) {
			descriptorSet->pDescriptorSetLayouts.push_back(
				*descriptorSetLayout);
		}

		descriptorSet->pMaxSets = pMaxSets;
		descriptorSet->init();
		descriptorSets.push_back(descriptorSet);

		return descriptorSet;
	}

	void VulkanShaderAttributes::clearVertexBindings() {
		vertexBindings.clear();
	}

	void VulkanShaderAttributes::clearVertexAttributes() {
		vertexAttributes.clear();
	}

	void VulkanShaderAttributes::clearDescriptors() {
		descriptors.clear();
	}

	void VulkanShaderAttributes::clearDescriptorSetLayouts() {
		descriptorSetLayouts.clear();
	}

	void VulkanShaderAttributes::clearDescriptorSets() {
		descriptorSets.clear();
	}

}