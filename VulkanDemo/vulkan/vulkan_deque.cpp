#include "vulkan_deque.h"

void VulkanDeque::addObject(VulkanObject* wrapper) {
	objects.push_back(wrapper);
}

void VulkanDeque::unwrap() {
	for (int i = objects.size() - 1; i >= 0; i--) {
		delete objects[i];
		objects.pop_back();
	}
}