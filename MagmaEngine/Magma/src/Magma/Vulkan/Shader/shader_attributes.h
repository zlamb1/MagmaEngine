#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include "vulkan/vulkan.h"

#include "Magma/Render/shader_attributes.h"

#include "Magma/Vulkan/Memory/descriptor.h"
#include "Magma/Vulkan/Memory/VMA/vma_buffer.h"
#include "Magma/Vulkan/Setup/vulkan_object.h"

namespace Magma {

	namespace Convert {

		static VkVertexInputRate convertVertexInputRate(VertexInputRate inputRate) {
			switch (inputRate) {
				case VertexInputRate::VERTEX:
					return VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
				case VertexInputRate::INSTANCE:
					return VkVertexInputRate::VK_VERTEX_INPUT_RATE_INSTANCE;
				default:
					return VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
			}
		}

		static VkFormat convertFormat(DataFormat format) {
			switch (format) {
				case DataFormat::R_SFLOAT32:
					return VkFormat::VK_FORMAT_R32_SFLOAT;
				case DataFormat::RG_SFLOAT32:
					return VkFormat::VK_FORMAT_R32G32_SFLOAT;
				case DataFormat::RGB_SFLOAT32:
					return VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
				case DataFormat::RGBA_SFLOAT32:
					return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;
				default:
					return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;
			}
		}

	}

	typedef VkVertexInputBindingDescription VkVertexBinding;
	typedef VkVertexInputAttributeDescription VkVertexAttribute;

	struct VulkanVertexBinding : public VertexBinding {
		VkVertexBinding getVertexBinding();
	};

	struct VulkanVertexAttribute : public VertexAttribute {
		VkVertexAttribute getVertexAttribute();
	};

	class VulkanShaderAttributes : public ShaderAttributes {

	public:
		VulkanShaderAttributes() = default;
		~VulkanShaderAttributes() = default;

		const std::vector<VulkanVertexBinding>& getVertexBindings();
		const std::vector<VulkanVertexAttribute>& getVertexAttributes();

		const std::vector<VkDescriptorSetLayout> getDescriptorSetLayouts();
		const std::vector<VkDescriptorSet> getDescriptorSets();

		VertexBinding createVertexBinding(uint32_t binding = 0,
			uint32_t stride = 0,
			VertexInputRate inputRate = VertexInputRate::VERTEX) override;

		VertexAttribute createVertexAttribute(uint32_t binding = 0,
			uint32_t location = 0,
			uint32_t offset = 0,
			DataFormat format = DataFormat::RGB_SFLOAT32) override;

		Descriptor createDescriptor(Buffer& pBuffer,
			uint32_t pBinding = 0,
			uint64_t pSize = 0, 
			uint32_t pCount = 1,
			VulkanShaderType pStageFlags = VulkanShaderType::VERTEX);

		std::shared_ptr<DescriptorSetLayout> createDescriptorSetLayout() override;

		std::shared_ptr<DescriptorSet> createDescriptorSet(uint32_t pMaxSets = 1) override;

		void clearVertexBindings();
		void clearVertexAttributes();

		void clearDescriptors();
		void clearDescriptorSetLayouts();
		void clearDescriptorSets();
	
	public:
		std::shared_ptr<VulkanDevice> pVulkanDevice = nullptr;

	private:
		std::vector<VulkanVertexBinding> vertexBindings{};
		std::vector<VulkanVertexAttribute> vertexAttributes{};

		std::vector<Descriptor> descriptors{};

		// these are shared pointers to manage their lifetime
		std::vector<std::shared_ptr<DescriptorSetLayout>> descriptorSetLayouts{};
		std::vector<std::shared_ptr<DescriptorSet>> descriptorSets{};

	};

}