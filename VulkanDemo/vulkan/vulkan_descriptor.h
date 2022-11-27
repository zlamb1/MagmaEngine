#pragma once

#include <vulkan/vulkan.h>

#include "vulkan_logger.h"
#include "vulkan_object.h"
#include "vulkan_shader.h"

class VulkanDescriptor : public VulkanObject {

public:
	VulkanDescriptor() = default;
	~VulkanDescriptor() override = default;

	uint32_t pBinding = 0, pCount = 1;
	VulkanShaderType pStageFlags = VulkanShaderType::VERTEX;

	VkResult init() override;

	VkDescriptorSetLayoutBinding& getLayoutBinding();

private:
	VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding{};

};

class VulkanDescriptorSetLayout : public VulkanObject {

public:
	VulkanDescriptorSetLayout(std::shared_ptr<VulkanDevice> pVulkanDevice);
	~VulkanDescriptorSetLayout() override;

	std::shared_ptr<VulkanDevice> pVulkanDevice = nullptr;

	std::vector<VkDescriptorSetLayoutBinding> pDescriptors{};

	VkResult init() override;

	VkDescriptorSetLayout& getDescriptorSetLayout();

private:
	VkDescriptorSetLayout vkDescriptorSetLayout{};

};

class VulkanDescriptorSet : public VulkanObject {


public:
	VulkanDescriptorSet() = default;
	~VulkanDescriptorSet() override;

	std::shared_ptr<VulkanDevice> pVulkanDevice = nullptr;

	uint32_t pMaxSets = 1;
	std::vector<VkDescriptorSetLayout> pDescriptorSetLayouts{};

	VkBuffer pBuffer;
	VkDeviceSize pSize = 0;

	VkResult init() override;

	std::vector<VkDescriptorSet>& getDescriptorSets();

private:
	VkDescriptorPool vkDescriptorPool{};
	std::vector<VkDescriptorSet> vkDescriptorSets{};

};