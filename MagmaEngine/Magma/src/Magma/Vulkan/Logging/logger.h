#pragma once

#include <any>
#include <iostream>
#include <string>
#include <vector>

class Logger {

public:
	Logger() = default;
	~Logger() = default;

	virtual void logText(const std::string& prefix, const std::string& text);
	virtual void logObject(const std::string& prefix, const std::any& object);

	void flush();

	const std::string& getApiName();
	void setApiName(std::string apiName);
	void setLogApiName(bool logApiName);

protected:
	std::string apiName = "[Logger]";
	bool logApiName = true;

	std::vector<std::string> logQueue{};

};