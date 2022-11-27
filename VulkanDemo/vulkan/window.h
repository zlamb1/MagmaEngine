#pragma once

#include <functional>
#include <string>
#include <utility>
#include <vector>

class Window {

public:
	Window() = default;
	virtual ~Window() = default;

	virtual void init(int32_t width, int32_t height);
	virtual void waitForEvents();

	// getters
	typedef std::pair<int32_t, int32_t> Int32Size;
	virtual bool shouldWindowClose();
	virtual Int32Size getMonitorResolution();
	virtual Int32Size getSize();
	virtual Int32Size getPosition();
	virtual const std::string& getTitle();
	virtual bool isResizable();
	virtual Int32Size getFramebufferSize();

	// setters
	virtual void setSize(int32_t width, int32_t height);
	virtual void setPosition(int32_t x, int32_t y);
	virtual void setTitle(std::string title);
	virtual void setResizable(bool resizable);
	virtual void onWindowResize(int32_t width, int32_t height);
	virtual void onFramebufferResize(int32_t width, int32_t height);

	// callbacks
	typedef std::function<void(int32_t, int32_t)> SizeCallback;
	void addWindowSizeCallback(SizeCallback callback); 
	void addFramebufferSizeCallback(SizeCallback callback);

private:
	std::vector<SizeCallback> windowSizeCallbacks{};
	std::vector<SizeCallback> framebufferSizeCallbacks{};

};