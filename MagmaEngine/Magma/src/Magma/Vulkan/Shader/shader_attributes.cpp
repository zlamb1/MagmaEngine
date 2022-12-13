#include "shader_attributes.h"

namespace Magma {

	VkVertexBinding VulkanVertexBinding::getVertexBinding() const {
		const VkVertexBinding vertexBinding{
			m_Binding, m_Stride, Convert::convertVertexInputRate(m_InputRate)
		};

		return vertexBinding;
	}

	VkVertexAttribute VulkanVertexAttribute::getVertexAttribute() const {
		const VkVertexAttribute vertexAttribute{
			m_Location, m_Binding, Convert::convertFormat(m_Format), m_Offset
		};

		return vertexAttribute;
	}

	const std::vector<VulkanVertexBinding>& VulkanShaderAttributes::getVertexBindings() {
		return m_VertexBindings;
	}

	const std::vector<VulkanVertexAttribute>& VulkanShaderAttributes::getVertexAttributes() {
		return m_VertexAttributes;
	}

	std::vector<VkDescriptorSetLayout> VulkanShaderAttributes::getDescriptorSetLayouts() const {
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{};
		for (auto& descriptorSetLayout : m_DescriptorSetLayouts) {
			descriptorSetLayouts.push_back(descriptorSetLayout->getDescriptorSetLayout());
		}

		return descriptorSetLayouts;
	}

	std::vector<VkDescriptorSet> VulkanShaderAttributes::getDescriptorSets() const {
		std::vector<VkDescriptorSet> descriptorSets{};
		for (const auto& descriptorSet : m_DescriptorSets) {
			auto& sets = descriptorSet->getDescriptorSets();
			descriptorSets.insert(descriptorSets.end(), sets.begin(), sets.end());
		}

		return descriptorSets;
	}

	VertexBinding VulkanShaderAttributes::createVertexBinding(const uint32_t binding, 
		const uint32_t stride, const VertexInputRate inputRate) {
		const VulkanVertexBinding vertexBinding{ binding, stride, inputRate };
		m_VertexBindings.push_back(vertexBinding);
		return vertexBinding;
	}

	VertexAttribute VulkanShaderAttributes::createVertexAttribute(const uint32_t binding, 
		const uint32_t location, const uint32_t offset, const DataFormat format) {
		const VulkanVertexAttribute vertexAttribute{ binding, location, offset, format };
		m_VertexAttributes.push_back(vertexAttribute);
		return vertexAttribute;
	}

	Descriptor VulkanShaderAttributes::createUniformDescriptor(const std::shared_ptr<Buffer> buffer,
		const uint32_t binding, const uint64_t size, const uint32_t count, const VulkanShaderType stageFlags) {
		Descriptor descriptor{};
		descriptor.pBinding = binding;
		descriptor.pCount = count;
		descriptor.pStageFlags = stageFlags;
		descriptor.pBuffer = buffer;
		descriptor.pSize = size;
		descriptor.init();
		m_Descriptors.push_back(descriptor);
		return descriptor;
	}

	Descriptor VulkanShaderAttributes::createImageDescriptor(const std::shared_ptr<VulkanImageView> imageView,
		const std::shared_ptr<Sampler> sampler, const uint32_t binding, const uint32_t count, 
		const VulkanShaderType stageFlags) {
		Descriptor descriptor{};
		descriptor.pDescriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; 
		descriptor.pBinding = binding;
		descriptor.pCount = count;
		descriptor.pStageFlags = stageFlags;
		descriptor.pImageView = imageView;
		descriptor.pSampler = sampler; 
		descriptor.init();
		m_Descriptors.push_back(descriptor);
		return descriptor;
	}

	std::shared_ptr<DescriptorSetLayout> VulkanShaderAttributes::createDescriptorSetLayout() {
		auto descriptorSetLayout = std::make_shared<DescriptorSetLayout>();
		descriptorSetLayout->pVulkanDevice = m_Device;
		for (auto& descriptor : m_Descriptors) {
			descriptorSetLayout->pDescriptors.push_back(descriptor);
		}
		descriptorSetLayout->init();
		m_DescriptorSetLayouts.push_back(descriptorSetLayout);
		return descriptorSetLayout;
	}

	std::shared_ptr<DescriptorSet> VulkanShaderAttributes::createDescriptorSet(uint32_t pMaxSets) {
		auto descriptorSet = std::make_shared<DescriptorSet>();
		descriptorSet->pVulkanDevice = m_Device;
		for (auto descriptorSetLayout : m_DescriptorSetLayouts) {
			descriptorSet->pDescriptorSetLayouts.push_back(descriptorSetLayout);
		}
		descriptorSet->pMaxSets = pMaxSets;
		descriptorSet->init();
		m_DescriptorSets.push_back(descriptorSet);
		return descriptorSet;
	}

	void VulkanShaderAttributes::clearVertexBindings() {
		m_VertexBindings.clear();
	}

	void VulkanShaderAttributes::clearVertexAttributes() {
		m_VertexAttributes.clear();
	}

	void VulkanShaderAttributes::clearDescriptors() {
		m_Descriptors.clear();
	}

	void VulkanShaderAttributes::clearDescriptorSetLayouts() {
		m_DescriptorSetLayouts.clear();
	}

	void VulkanShaderAttributes::clearDescriptorSets() {
		m_DescriptorSets.clear();
	}

}