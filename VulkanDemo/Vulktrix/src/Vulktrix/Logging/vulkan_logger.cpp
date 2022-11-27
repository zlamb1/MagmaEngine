#include "vulkan_logger.h"

VulkanLogger& VulkanLogger::instance() {
	static VulkanLogger instance;
	return instance;
}

void VulkanLogger::enqueueText(const std::string& prefix, const std::string& text) {
	SingletonLogger::enqueueText(prefix, text);
	SingletonLogger::flushQueue();
}

void VulkanLogger::enqueueObject(const std::string& prefix, const std::any& object) {
	std::string typeName = object.type().name();
	if (typeName == "enum VkResult") {
		auto index = (int)std::any_cast<VkResult>(object);
		if (logSuccess || index != 0) {
			enqueueText(prefix, ERROR_NAMES.at(index));
		}
	} else {
		enqueueText(prefix, "");
	}
}