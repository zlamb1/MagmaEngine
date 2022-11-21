#include "vk_deque.h"

VulkanDeque::VulkanDeque() {

}

void VulkanDeque::addWrapper(VulkanWrapper* wrapper) {
	wrappers.push_back(wrapper);
}

void VulkanDeque::unwrap() {
	for (int i = wrappers.size() - 1; i >= 0; i--) {
		delete wrappers[i];
		wrappers.pop_back();
	}
}