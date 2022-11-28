#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include "vulkan/vulkan.h"

#include "Vulktrix/Memory/descriptor.h"

#include "Vulktrix/Setup/vulkan_object.h"

enum class VulkanFormat {
	R_SFLOAT32 = VK_FORMAT_R32_SFLOAT,
	RG_SFLOAT32 = VK_FORMAT_R32G32_SFLOAT,
	RGB_SFLOAT32 = VK_FORMAT_R32G32B32_SFLOAT,
	RGBA_SFLOAT32 = VK_FORMAT_R32G32B32A32_SFLOAT,
};

enum class VertexInputRate {
	VERTEX = VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX,
	INSTANCE = VkVertexInputRate::VK_VERTEX_INPUT_RATE_INSTANCE
};

class ShaderAttributes {

public:
	ShaderAttributes() = default;
	~ShaderAttributes() = default;

	std::shared_ptr<VulkanDevice> pVulkanDevice = nullptr;

	typedef VkVertexInputBindingDescription VkVertexBinding;
	typedef VkVertexInputAttributeDescription VkVertexAttribute;

	const std::vector<VkVertexBinding>& getVertexBindings();
	const std::vector<VkVertexAttribute>& getVertexAttributes();

	const std::vector<VkDescriptorSetLayout> getDescriptorSetLayouts();
	const std::vector<VkDescriptorSet> getDescriptorSets();

	VkVertexBinding createVertexBinding(uint32_t binding = 0,
		uint32_t stride = 0, 
		VertexInputRate inputRate = VertexInputRate::VERTEX);

	VkVertexAttribute createVertexAttribute(uint32_t binding = 0,
		uint32_t location = 0,
		uint32_t offset = 0, 
		VulkanFormat format = VulkanFormat::RGB_SFLOAT32);

	Descriptor createDescriptor(uint32_t pBinding = 0, uint32_t pCount = 1,
		VulkanShaderType pStageFlags = VulkanShaderType::VERTEX);

	std::shared_ptr<DescriptorSetLayout> createDescriptorSetLayout();

	std::shared_ptr<DescriptorSet> createDescriptorSet(VkBuffer& pBuffer, 
		uint32_t pMaxSets = 1, VkDeviceSize pSize = 0);

	void clearVertexBindings();
	void clearVertexAttributes();

	void clearDescriptors();
	void clearDescriptorSetLayouts();
	void clearDescriptorSets();

private:
	std::vector<VkVertexBinding> vertexBindings{};
	std::vector<VkVertexAttribute> vertexAttributes{};

	std::vector<Descriptor> vulkanDescriptors{};

	// these are shared pointers to manage their lifetime
	std::vector<std::shared_ptr<DescriptorSetLayout>> descriptorSetLayouts{};
	std::vector<std::shared_ptr<DescriptorSet>> descriptorSets{};

};