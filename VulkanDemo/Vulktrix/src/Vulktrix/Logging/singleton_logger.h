#pragma once

#include <any>
#include <iostream>
#include <string>
#include <vector>

class SingletonLogger {

public:
	SingletonLogger() = default;
	~SingletonLogger() = default;

	virtual void enqueueText(const std::string& prefix, const std::string& text);
	virtual void enqueueObject(const std::string& prefix, const std::any& object);

	void flushQueue();

	const std::string& getApiName();
	void setApiName(std::string apiName);
	void setLogApiName(bool logApiName);

protected:
	std::string apiName = "[Logger]";
	bool logApiName = true;

	std::vector<std::string> logQueue{};

};