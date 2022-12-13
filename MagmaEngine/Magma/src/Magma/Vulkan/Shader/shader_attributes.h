#pragma once

#include <memory>
#include <vector>

#include "Magma/Render/shader_attributes.h"

#include "Magma/Vulkan/Memory/descriptor.h"
#include "Magma/Vulkan/Memory/VMA/vma_buffer.h"

namespace Magma {

	namespace Convert {

		static VkVertexInputRate convertVertexInputRate(const VertexInputRate inputRate) {
			switch (inputRate) {
				case VertexInputRate::VERTEX:
					return VK_VERTEX_INPUT_RATE_VERTEX;
				case VertexInputRate::INSTANCE:
					return VK_VERTEX_INPUT_RATE_INSTANCE;
			}

			return VK_VERTEX_INPUT_RATE_VERTEX; 
		}

		static VkFormat convertFormat(const DataFormat format) {
			switch (format) {
				case DataFormat::R_SFLOAT32:
					return VK_FORMAT_R32_SFLOAT;
				case DataFormat::RG_SFLOAT32:
					return VK_FORMAT_R32G32_SFLOAT;
				case DataFormat::RGB_SFLOAT32:
					return VK_FORMAT_R32G32B32_SFLOAT;
				case DataFormat::RGBA_SFLOAT32:
					return VK_FORMAT_R32G32B32A32_SFLOAT;
			}

			return VK_FORMAT_R32G32B32A32_SFLOAT;
		}

	}

	typedef VkVertexInputBindingDescription VkVertexBinding;
	typedef VkVertexInputAttributeDescription VkVertexAttribute;

	struct VulkanVertexBinding : VertexBinding {
		[[nodiscard]] VkVertexBinding getVertexBinding() const;
	};

	struct VulkanVertexAttribute : VertexAttribute {
		[[nodiscard]] VkVertexAttribute getVertexAttribute() const;
	};

	class VulkanShaderAttributes : public ShaderAttributes {

	public:
		VulkanShaderAttributes() = default;
		virtual ~VulkanShaderAttributes() = default;

		const std::vector<VulkanVertexBinding>& getVertexBindings();
		const std::vector<VulkanVertexAttribute>& getVertexAttributes();

		[[nodiscard]] std::vector<VkDescriptorSetLayout> getDescriptorSetLayouts() const;
		[[nodiscard]] std::vector<VkDescriptorSet> getDescriptorSets() const;

		VertexBinding createVertexBinding(uint32_t binding = 0,
			uint32_t stride = 0,
			VertexInputRate inputRate = VertexInputRate::VERTEX) override;
		VertexAttribute createVertexAttribute(uint32_t binding = 0,
			uint32_t location = 0,
			uint32_t offset = 0,
			DataFormat format = DataFormat::RGB_SFLOAT32) override;

		Descriptor createUniformDescriptor(std::shared_ptr<Buffer> buffer,
			uint32_t binding = 0, uint64_t size = 0, uint32_t count = 1,
			VulkanShaderType stageFlags = VulkanShaderType::VERTEX) override;

		Descriptor createImageDescriptor(std::shared_ptr<VulkanImageView> imageView,
			std::shared_ptr<Sampler> sampler, uint32_t binding = 0, uint32_t count = 1,
			VulkanShaderType stageFlags = VulkanShaderType::VERTEX) override;

		std::shared_ptr<DescriptorSetLayout> createDescriptorSetLayout() override;

		std::shared_ptr<DescriptorSet> createDescriptorSet(uint32_t pMaxSets = 1) override;

		void clearVertexBindings();
		void clearVertexAttributes();

		void clearDescriptors();
		void clearDescriptorSetLayouts();
		void clearDescriptorSets();
	
	public:
		std::shared_ptr<VulkanDevice> m_Device = nullptr;

	private:
		std::vector<VulkanVertexBinding> m_VertexBindings{};
		std::vector<VulkanVertexAttribute> m_VertexAttributes{};

		std::vector<Descriptor> m_Descriptors{};

		// these are shared pointers to manage their lifetime
		std::vector<std::shared_ptr<DescriptorSetLayout>> m_DescriptorSetLayouts{};
		std::vector<std::shared_ptr<DescriptorSet>> m_DescriptorSets{};

	};

}