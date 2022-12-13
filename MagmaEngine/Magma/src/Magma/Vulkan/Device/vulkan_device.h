#pragma once

#include "device_profile.h"

#include "Magma/Vulkan/Memory/VMA/memory_allocator.h"
#include "Magma/Vulkan/Setup/vulkan_validater.h"
#include "Magma/Vulkan/Setup/vulkan_instance.h"

namespace Magma {
	
	class VulkanDevice : public VulkanObject {

	public:
		VulkanDevice(std::shared_ptr<VulkanInstance> pVulkanInstance,
			std::shared_ptr<Surface> surface, std::shared_ptr<VulkanValidater> pValidater);
		~VulkanDevice() override;

		VkResult init() override;

		VkPhysicalDevice& getPhysicalDevice();
		VkDevice& getDevice();

		[[nodiscard]] const MemoryAllocator* getMemoryAllocator() const;

		SwapchainSupportDetails querySwapchainSupport();

	public:
		std::shared_ptr<VulkanInstance> pVulkanInstance = nullptr;
		std::shared_ptr<Surface> pVulkanSurface = nullptr;
		std::shared_ptr<VulkanValidater> pVulkanValidater = nullptr;

	private:
		VkPhysicalDevice physicalDevice{};
		VkDevice device{};
		MemoryAllocator* allocator = nullptr; 

	};

}