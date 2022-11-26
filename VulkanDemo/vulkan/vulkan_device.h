#pragma once

#include <iostream>  
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

#include <vulkan/vulkan.h>

#include "vulkan_logger.h"
#include "vulkan_surface.h"
#include "vulkan_validater.h"
#include "vulkan_object.h"
#include "vulkan_instance.h"

class QueueFamily {

public:
	uint32_t getQueue();
	uint32_t getPresentation();

	void setQueue();
	void setQueue(uint32_t queue);

	void setPresentation();
	void setPresentation(uint32_t presentation);

	bool hasQueue();
	bool hasPresentation();

private:
	std::optional<uint32_t> queue, presentation;

};

class SwapchainSupportDetails {

public:
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats{};
	std::vector<VkPresentModeKHR> presentModes{};

};

class VulkanDevice : public VulkanObject {

public:
	VulkanDevice(std::shared_ptr<VulkanInstance> pVulkanInstance, 
		std::shared_ptr<VulkanSurface> pVulkanSurface, std::shared_ptr<VulkanValidater> pValidater);
	~VulkanDevice();

	std::shared_ptr<VulkanInstance> pVulkanInstance = nullptr;
	std::shared_ptr<VulkanSurface> pVulkanSurface = nullptr; 
	std::shared_ptr<VulkanValidater> pVulkanValidater = nullptr;

	VkResult init();

	VkPhysicalDevice& getPhysicalDevice();
	VkDevice& getDevice();

	SwapchainSupportDetails querySwapchainSupport();

private:
	VkPhysicalDevice physicalDevice{};
	VkDevice device{};

};

enum class VulkanPriority {
	NONE = 0,
	LOW = 1,
	MEDIUM = 2,
	HIGH = 3,
	REQUIRED = -1
};

enum class VulkanDeviceType {
	CPU = VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_CPU,
	DISCRETE_GPU = VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
	INTEGRATED_GPU = VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU,
	MAX = VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM,
	OTHER = VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_OTHER,
	VIRTUAL_GPU = VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU
};

enum class VulkanQueueType {
	COMPUTE = VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT,
	GRAPHICS = VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT,
	TRANSFER = VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT
};

inline VulkanQueueType operator|(VulkanQueueType& a, VulkanQueueType& b) {
	return (VulkanQueueType)((uint32_t)a | (uint32_t)b);
}

enum class VulkanDeviceFeature {
	robustBufferAccess,
	fullDrawIndexUint32,
	imageCubeArray,
	independentBlend,
	geometryShader,
	tessellationShader,
	sampleRateShading,
	dualSrcBlend,
	logicOp,
	multiDrawIndirect,
	drawIndirectFirstInstance,
	depthClamp,
	depthBiasClamp,
	fillModeNonSolid,
	depthBounds,
	wideLines,
	largePoints,
	alphaToOne,
	multiViewport,
	samplerAnisotropy,
	textureCompressionETC2,
	textureCompressionASTC_LDR,
	textureCompressionBC,
	occlusionQueryPrecise,
	pipelineStatisticsQuery,
    vertexPipelineStoresAndAtomics,
    fragmentStoresAndAtomics,
    shaderTessellationAndGeometryPointSize,
    shaderImageGatherExtended,
    shaderStorageImageExtendedFormats,
    shaderStorageImageMultisample,
    shaderStorageImageReadWithoutFormat,
    shaderStorageImageWriteWithoutFormat,
    shaderUniformBufferArrayDynamicIndexing,
    shaderSampledImageArrayDynamicIndexing,
    shaderStorageBufferArrayDynamicIndexing,
    shaderStorageImageArrayDynamicIndexing,
    shaderClipDistance,
    shaderCullDistance,
    shaderFloat64,
    shaderInt64,
    shaderInt16,
    shaderResourceResidency,
    shaderResourceMinLod,
    sparseBinding,
    sparseResidencyBuffer,
    sparseResidencyImage2D,
    sparseResidencyImage3D,
    sparseResidency2Samples,
    sparseResidency4Samples,
    sparseResidency8Samples,
    sparseResidency16Samples,
    sparseResidencyAliased,
    variableMultisampleRate,
    inheritedQueries
};

class VulkanDeviceProfile {

public:
	static VulkanDeviceProfile& instance(); 
	
	VkPhysicalDevice& getDevice(); 
	VulkanDeviceType getDeviceType();

	std::map<VulkanQueueType, std::optional<VulkanPriority>>& getQueueTypes();
	std::map<VulkanQueueType, std::optional<uint32_t>>& getQueueIndices();
	std::optional<VkQueue> getQueue(VulkanQueueType vulkanQueueType);

	std::vector<const char*>& getRequiredExtensions();

	void generateProfile(VkPhysicalDevice vkPhysicalDevice); 

	void setDevice(VkDevice vkDevice);
	void setVulkanSurface(std::shared_ptr<VulkanSurface> pVulkanSurface);

	void setDeviceTypePriority(VulkanPriority vulkanDeviceTypePriority);
	void setDeviceType(VulkanDeviceType vulkanDeviceType);

	void setQueueType(VulkanQueueType vulkanQueueType, VulkanPriority queueTypePriority);
	void setQueueSupportsPresentation(VulkanQueueType vulkanQueueType, VkBool32 val);

	int32_t getDeviceScore(VkPhysicalDevice& vkPhysicalDevice);

	static QueueFamily findQueueFamily(VkPhysicalDevice& device, VkSurfaceKHR& surface,
		VulkanQueueType flags, VkBool32 mustSupportPresentation);

private:
	VulkanDeviceProfile() = default;
	~VulkanDeviceProfile() = default;
	VulkanDeviceProfile& operator=(VulkanDeviceProfile& o) = default;

	VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE;
	VkDevice vkDevice = VK_NULL_HANDLE;

	std::shared_ptr<VulkanSurface> pVulkanSurface = nullptr;

	VulkanPriority vulkanDeviceTypePriority = VulkanPriority::HIGH;
	VulkanDeviceType vulkanDeviceType = VulkanDeviceType::DISCRETE_GPU;

	// by default the device requires a graphics queue
	std::map<VulkanQueueType, std::optional<VulkanPriority>> deviceQueueTypes{  
		{ VulkanQueueType::GRAPHICS, VulkanPriority::REQUIRED }
	};

	// by default the graphics queue must support presentation
	std::map<VulkanQueueType, VkBool32> queueSupportsPresentation{ 
		{ VulkanQueueType::GRAPHICS, VK_TRUE } 
	};

	// queues are generated on profile generate
	std::map<VulkanQueueType, std::optional<uint32_t>> queueIndices{};
	std::map<VulkanQueueType, std::optional<VkQueue>> queues{};

	std::vector<const char*> vulkanDeviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	std::vector<std::pair<VulkanDeviceFeature, VulkanPriority>> vulkanDeviceFeatures{};

};