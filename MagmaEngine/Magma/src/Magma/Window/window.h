#pragma once

#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace Magma {

	enum class MouseButton : int {
		LEFT,
		RIGHT,
		UNKNOWN
	};

	enum class MouseAction : int {
		RELEASE,
		PRESS,
		UNKNOWN
	};

	typedef std::pair<int32_t, int32_t> Int32Size;
	typedef std::pair<int32_t, int32_t> Int32Pos;

	typedef std::function<void(int32_t, int32_t)> SizeCallback;
	typedef std::function<void(double, double)> PosCallback;
	typedef std::function<void(double, double)> OffsetCallback;
	typedef std::function<void(MouseButton button, bool pressed, int mods)> ButtonCallback;
	typedef std::function<void()> VoidCallback;

	class WindowImpl {

	public:
		WindowImpl() = default;
		virtual ~WindowImpl() = default;

		virtual void init(int32_t width, int32_t height) = 0;

		virtual void waitForEvents() = 0;

		virtual bool shouldWindowClose() = 0;
		virtual Int32Size getMonitorResolution() = 0;
		virtual Int32Size getSize() = 0;
		virtual Int32Pos getPosition() = 0;
		virtual const std::string& getTitle() = 0;
		virtual bool isResizable() = 0;
		virtual Int32Size getFramebufferSize() = 0;

		virtual void setSize(int32_t width, int32_t height) = 0;
		virtual void setPosition(int32_t x, int32_t y) = 0;
		virtual void setTitle(std::string title) = 0;
		virtual void setResizable(bool resizable) = 0;
		virtual void setMousePosition(double x, double y) = 0;
		virtual void setMouseHidden(bool enabled) = 0;

		virtual void onWindowResize(int32_t width, int32_t height);
		virtual void onFramebufferResize(int32_t width, int32_t height);
		virtual void onMouseScroll(double x, double y);
		virtual void onMouseMove(int32_t x, int32_t y);
		virtual void onMouseEnter();
		virtual void onMouseExit();
		virtual void onMousePress(MouseButton button, bool pressed, int mods);

		void addWindowSizeCallback(SizeCallback callback);
		void addFramebufferSizeCallback(SizeCallback callback);
		void addMousePosCallback(PosCallback callback);
		void addMouseScrollCallback(OffsetCallback callback);
		void addMouseButtonCallback(ButtonCallback callback);
		void addMouseEnterCallback(VoidCallback callback);
		void addMouseExitCallback(VoidCallback callback);

		virtual MouseButton getMouseButton(int btn);
		virtual MouseAction getMouseAction(int action);

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