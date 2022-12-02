#include "vulkan_logger.h"

namespace Magma {

	VulkanLogger& VulkanLogger::instance() {
		static VulkanLogger instance;
		return instance;
	}

	void VulkanLogger::logText(const std::string& prefix, const std::string& text) {
		Logger::logText(prefix, text);
		Logger::flush();
	}

	void VulkanLogger::logObject(const std::string& prefix, const std::any& object) {
		std::string typeName = object.type().name();
		if (typeName == "enum VkResult") {
			auto index = (int)std::any_cast<VkResult>(object);
			if (logSuccess || index != 0) {
				logText(prefix, ERROR_NAMES.at(index));
			}
		}
		else {
			logText(prefix, "");
		}
	}

}