#include "memory_allocator.h"

namespace Magma {

	MemoryAllocator::MemoryAllocator(const VkPhysicalDevice& physicalDevice, const VkDevice& device,
		const VkInstance& instance) {
		VmaAllocatorCreateInfo allocatorCreateInfo = {};
		allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
		allocatorCreateInfo.physicalDevice = physicalDevice;
		allocatorCreateInfo.device = device;
		allocatorCreateInfo.instance = instance;
		// Vulkan is linked statically, so VMA will use the statically linked functions automatically
		// allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;
		vmaCreateAllocator(&allocatorCreateInfo, &vmaAllocator);
	}

	MemoryAllocator::~MemoryAllocator() {
		vmaDestroyAllocator(vmaAllocator);
	}

	const VmaAllocator& MemoryAllocator::getAllocator() const {
		return vmaAllocator;
	}

}