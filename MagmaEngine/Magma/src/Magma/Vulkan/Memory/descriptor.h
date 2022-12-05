#pragma once

#include "Magma/Vulkan/Shader/vulkan_shader.h"

namespace Magma {

	class Descriptor : public VulkanObject {

	public:
		Descriptor() = default;
		~Descriptor() override = default;

		VkResult init() override;

		VkDescriptorSetLayoutBinding& getLayoutBinding();

	public:
		uint32_t pBinding = 0, pCount = 1;
		VulkanShaderType pStageFlags = VulkanShaderType::VERTEX;

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

		std::vector<VkDescriptorSetLayoutBinding> pDescriptors{};

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
		std::vector<VkDescriptorSetLayout> pDescriptorSetLayouts{};

		VkBuffer pBuffer{};
		VkDeviceSize pSize = 0;

	private:
		VkDescriptorPool vkDescriptorPool{};
		std::vector<VkDescriptorSet> vkDescriptorSets{};

	};

}