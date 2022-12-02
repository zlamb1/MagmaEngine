#pragma once

#include <map>

#include <fstream>
#include <iostream>

#include <vulkan/vulkan.h>

namespace Magma {

	class IO {
		public:
			static IO& instance() {
				static IO io;
				return io;
			}

			static void writeText(std::string prefix, std::string text) {
				IO& io = IO::instance();
				io.fileStream << prefix << " => " << text << std::endl;
			}

			static void writeResult(std::string prefix, VkResult result) {
				IO& io = IO::instance();
				writeText(prefix, io.ERROR_NAMES.at((int)result));
			}
		
		public:
			std::string file = "log.txt";
			std::ofstream fileStream{ file };

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

		private:
			IO() = default;
			~IO() = default;
			IO& operator=(const IO& io) = default;
	};

}