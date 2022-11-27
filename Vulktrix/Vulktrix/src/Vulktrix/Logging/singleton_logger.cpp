#include "singleton_logger.h"

void SingletonLogger::enqueueText(const std::string& prefix, const std::string& text) {
	logQueue.push_back(prefix + " => " + text);
}

void SingletonLogger::enqueueObject(const std::string& prefix, const std::any& object) {
	// not yet implemented
}

void SingletonLogger::flushQueue() {
	for (auto& text : logQueue) {
		std::cout << apiName << " " << text << std::endl;
	}
	logQueue.clear();
}

const std::string& SingletonLogger::getApiName() {
	return apiName;
}

void SingletonLogger::setApiName(std::string apiName) {
	this->apiName = apiName;
}

void SingletonLogger::setLogApiName(bool logApiName) {
	this->logApiName = logApiName;
}