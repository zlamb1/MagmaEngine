#pragma once

#include <iostream>
#include <map>
#include <string>

#include <vulkan/vulkan.h>

#include "logger.h"

#include "Magma/Vulkan/IO/io.h"

#ifndef DISTRIBUTION
	#define Z_LOG_TXT(x, y) VulkanLogger::instance().logText(x, y)
	#define Z_LOG_OBJ(x, y) VulkanLogger::instance().logObject(x, y)
#else
	// TODO: log to file
	#define Z_LOG_TXT(x, y) Magma::IO::writeText(x, y)
	#define Z_LOG_OBJ(x, y) Magma::IO::writeResult(x, y)
#endif

namespace Magma {

	class VulkanLogger : public Logger {

	public:
		static VulkanLogger& instance();

		void logText(const std::string& prefix, const std::string& text) override;
		void logObject(const std::string& prefix, const std::any& object) override;

	private:
		VulkanLogger() { apiName = "[VulkanAPI]"; };
		~VulkanLogger() = default;
		VulkanLogger& operator=(VulkanLogger& o) = default;

		bool logSuccess = false;

		const std::map<int, const std::string> ERROR_NAMES{
			{0, "VK_SUCCESS"},
			{1, "VK_NOT_READY"},
			{2, "VK_TIMEOUT"},
			{3, "VK_EVENT_SET"},
			{4, "VK_EVENT_RESET"},
			{5, "VK_INCOMPLETE"},
			{-1, "VK_ERROR_OUT_OF_HOST_MEMORY"},
			{-2, "VK_ERROR_OUT_OF_DEVICE_MEMORY"},
			{-3, "VK_ERROR_INITIALIZATION_FAILED"},
			{-4, "VK_ERROR_DEVICE_LOST"},
			{-5, "VK_ERROR_MEMORY_MAP_FAILED"},
			{-6, "VK_ERROR_LAYER_NOT_PRESENT"},
			{-7, "VK_ERROR_EXTENSION_NOT_PRESENT"},
			{-8, "VK_ERROR_FEATURE_NOT_PRESENT"},
			{-9, "VK_ERROR_INCOMPATIBLE_DRIVER"},
			{-10, "VK_ERROR_TOO_MANY_OBJECTS"},
			{-11, "VK_ERROR_FORMAT_NOT_SUPPORTED"},
			{-12, "VK_ERROR_FRAGMENTED_POOL"},
			{-13, "VK_ERROR_UNKNOWN"}
		};

	};

}