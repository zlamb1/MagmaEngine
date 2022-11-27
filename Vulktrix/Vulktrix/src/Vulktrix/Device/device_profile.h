#pragma once

#include <set>

#include "device_enums.h"

#include "Vulktrix/Surface/vulkan_surface.h"

class DeviceProfile {

public:
	static DeviceProfile& instance();
    static std::optional<VkQueue> getQueue(VulkanQueueType vulkanQueueType);

	VkPhysicalDevice& getDevice();
	VulkanDeviceType getDeviceType();

	std::map<VulkanQueueType, std::optional<uint32_t>>& getQueueIndices();
	std::optional<VkQueue> getVkQueue(VulkanQueueType vulkanQueueType);

	std::vector<const char*>& getRequiredExtensions();

	void generateProfile(VkPhysicalDevice vkPhysicalDevice);

	void setDevice(VkDevice vkDevice);
	void setVulkanSurface(std::shared_ptr<VulkanSurface> pVulkanSurface);

	void setDeviceTypePriority(VulkanPriority vulkanDeviceTypePriority);
	void setDeviceType(VulkanDeviceType vulkanDeviceType);

	void setQueueType(VulkanQueueType vulkanQueueType, VulkanPriority queueTypePriority);
	void setQueueSupportsPresentation(VulkanQueueType vulkanQueueType, VkBool32 val);

	int32_t getDeviceScore(VkPhysicalDevice& vkPhysicalDevice);

	static QueueFamily findQueueFamily(VkPhysicalDevice& device, VkSurfaceKHR& surface, VulkanQueueType flags, 
		VkBool32 mustSupportPresentation);
	static SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice& device, VkSurfaceKHR& surface);
	static bool checkDeviceExtensionsSupport(VkPhysicalDevice& device, std::vector<const char*> requiredExtensions);

private:
	DeviceProfile() = default;
	~DeviceProfile() = default;
	DeviceProfile& operator=(DeviceProfile& o) = default;

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