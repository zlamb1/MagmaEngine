#pragma once

#include "vk_mem_alloc.h"

namespace Magma {
	class MemoryAllocator {

	public:
		MemoryAllocator(const VkPhysicalDevice& pPhysicalDevice, const VkDevice& device, const VkInstance& instance);
		~MemoryAllocator();

		const VmaAllocator& getAllocator() const;

	private:
		VmaAllocator vmaAllocator; 

	};
}

