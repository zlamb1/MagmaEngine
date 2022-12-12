#pragma once

#include "VMA/vma_buffer.h"

#include "Magma/Vulkan/Image/image_view.h"
#include "Magma/Vulkan/Image/sampler.h"
#include "Magma/Vulkan/Shader/vulkan_shader.h"

namespace Magma {

	class Descriptor : public VulkanObject {

	public:
		Descriptor() = default;
		~Descriptor() override = default;

		VkResult init() override;

		VkDescriptorSetLayoutBinding& getLayoutBinding();

	public:
		VkDescriptorType pDescriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uint32_t pBinding = 0, pCount = 1;
		VulkanShaderType pStageFlags = VulkanShaderType::VERTEX;

		std::shared_ptr<Buffer> pBuffer;
		VkDeviceSize pSize = 0;

		std::shared_ptr<VulkanImageView> pImageView = nullptr;
		std::shared_ptr<Sampler> pSampler = nullptr; 

	private:
		VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding{};

	};

	class DescriptorSetLayout : public VulkanObject {

	public:
		DescriptorSetLayout() = default;
		~DescriptorSetLayout() override;

		VkResult init() override;

		VkDescriptorSetLayout& getDescriptorSetLayout();

	public:
		std::shared_ptr<VulkanDevice> pVulkanDevice = nullptr;
		std::vector<Descriptor> pDescriptors{};

	private:
		VkDescriptorSetLayout vkDescriptorSetLayout{};

	};

	class DescriptorSet : public VulkanObject {

	public:
		DescriptorSet() = default;
		~DescriptorSet() override;

		VkResult init() override;

		std::vector<VkDescriptorSet>& getDescriptorSets();
	
	public:
		std::shared_ptr<VulkanDevice> pVulkanDevice = nullptr;

		uint32_t pMaxSets = 1;
		std::vector<std::shared_ptr<DescriptorSetLayout>> pDescriptorSetLayouts{};

	private:
		VkDescriptorPool vkDescriptorPool{};
		std::vector<VkDescriptorSet> vkDescriptorSets{};

	};

}