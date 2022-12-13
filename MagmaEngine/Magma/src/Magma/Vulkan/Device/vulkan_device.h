#pragma once

#include "device_profile.h"

#include "Magma/Vulkan/Memory/VMA/memory_allocator.h"
#include "Magma/Vulkan/Setup/validater.h"
#include "Magma/Vulkan/Setup/instance.h"

namespace Magma {
	
	class VulkanDevice : public VulkanObject {

	public:
		VulkanDevice(std::shared_ptr<Instance> pVulkanInstance,
			std::shared_ptr<Surface> surface, std::shared_ptr<Validater> pValidater);
		~VulkanDevice() override;

		VkResult init() override;

		VkPhysicalDevice& getPhysicalDevice();
		VkDevice& getDevice();

		[[nodiscard]] const MemoryAllocator* getMemoryAllocator() const;

		SwapchainSupportDetails querySwapchainSupport();

	public:
		std::shared_ptr<Instance> pVulkanInstance = nullptr;
		std::shared_ptr<Surface> pVulkanSurface = nullptr;
		std::shared_ptr<Validater> pVulkanValidater = nullptr;

	private:
		VkPhysicalDevice physicalDevice{};
		VkDevice device{};
		MemoryAllocator* allocator = nullptr; 

	};

}