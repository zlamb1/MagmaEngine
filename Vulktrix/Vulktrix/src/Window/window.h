#pragma once

#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace Window {

	enum MouseButton : int;
	enum MouseAction : int;

	typedef std::pair<int32_t, int32_t> Int32Size;

	typedef std::function<void(int32_t, int32_t)> SizeCallback;
	typedef std::function<void(double, double)> PosCallback;
	typedef std::function<void(double, double)> OffsetCallback;
	typedef std::function<void(MouseButton button, MouseAction action, int mods)> ButtonCallback;
	typedef std::function<void()> VoidCallback;

	class WindowImpl {

	public:
		WindowImpl() = default;
		virtual ~WindowImpl() = default;

		virtual void init(int32_t width, int32_t height);

		virtual void waitForEvents();

		// getters
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
		virtual void setMousePosition(double x, double y);
		virtual void setMouseHidden(bool enabled);

		// callbacks
		virtual void onWindowResize(int32_t width, int32_t height);
		virtual void onFramebufferResize(int32_t width, int32_t height);

		virtual void onMouseScroll(double x, double y);

		virtual void onMouseMove(int32_t x, int32_t y);

		virtual void onMouseEnter();
		virtual void onMouseExit();

		virtual void onMousePress(MouseButton button, MouseAction action, int mods);

		// callback adders

		void addWindowSizeCallback(SizeCallback callback);
		void addFramebufferSizeCallback(SizeCallback callback);

		void addMousePosCallback(PosCallback callback);

		void addMouseScrollCallback(OffsetCallback callback);

		void addMouseButtonCallback(ButtonCallback callback);

		void addMouseEnterCallback(VoidCallback callback);
		void addMouseExitCallback(VoidCallback callback);

	private:
		std::vector<SizeCallback> windowSizeCallbacks{};
		std::vector<SizeCallback> framebufferSizeCallbacks{};

		std::vector<PosCallback> mousePosCallbacks{};

		std::vector<OffsetCallback> mouseScrollCallbacks{};

		std::vector<ButtonCallback> mouseButtonCallbacks{};

		std::vector<VoidCallback> mouseEnterCallbacks{};
		std::vector<VoidCallback> mouseExitCallbacks{};

	};
}