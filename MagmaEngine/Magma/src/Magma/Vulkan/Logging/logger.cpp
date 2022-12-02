#include "logger.h"

void Logger::logText(const std::string& prefix, const std::string& text) {
	logQueue.push_back(prefix + " => " + text);
}

void Logger::logObject(const std::string& prefix, const std::any& object) {
	// not yet implemented
}

void Logger::flush() {
	for (auto& text : logQueue) {
		std::cout << apiName << " " << text << std::endl;
	}
	logQueue.clear();
}

const std::string& Logger::getApiName() {
	return apiName;
}

void Logger::setApiName(std::string apiName) {
	this->apiName = apiName;
}

void Logger::setLogApiName(bool logApiName) {
	this->logApiName = logApiName;
}