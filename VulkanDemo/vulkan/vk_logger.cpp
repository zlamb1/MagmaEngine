#include "vk_logger.h"

_VkLogger& _VkLogger::Instance() {
	static _VkLogger logger; 
	return logger; 
}

void _VkLogger::SetLogging(bool doLogging) {
	this->doLogging = doLogging;
}

void _VkLogger::LogText(std::string text) {
	if (doLogging) {
		std::cout << API_NAME << " " << text << std::endl;
	}
}

void _VkLogger::LogResult(std::string text, VkResult result) {
	if (doLogging) {
		std::cout << API_NAME << " " << text << " " << ERROR_NAMES.at(result) << std::endl;
	}
}