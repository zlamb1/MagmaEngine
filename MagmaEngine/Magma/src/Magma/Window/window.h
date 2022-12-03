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

	enum class KeyAction : int {
		RELEASE,
		PRESS,
		REPEAT,
		UNKNOWN
	};

	enum class KeyButton : int {
		TAB, Q, W, E, R, T, Y, U, I, O, P, LEFT_BRACKET, RIGHT_BRACKET, BACKSLASH,
		CAPS_LOCK, A, S, D, F, G, H, J, K, L, SEMICOLON, APOSTROPHE, ENTER,
		LEFT_SHIFT, Z, X, C, V, B, N, M, COMMA, PERIOD, SLASH, RIGHT_SHIFT,
		LEFT_CONTROL, LEFT_ALT, SPACE, RIGHT_ALT, RIGHT_CONTROL, UNKNOWN
	};

	typedef std::pair<int32_t, int32_t> Int32Size;
	typedef std::pair<int32_t, int32_t> Int32Pos;

	typedef std::pair<double, double> DoublePos; 

	typedef std::function<void(int32_t, int32_t)> SizeCallback;
	typedef std::function<void(int)> BoolCallback;
	typedef std::function<void(double, double)> PosCallback;
	typedef std::function<void(double, double)> OffsetCallback;
	typedef std::function<void(MouseButton button, bool pressed, int mods)> MouseButtonCallback;
	typedef std::function<void(KeyButton button, KeyAction action)> KeyPressCallback;
	typedef std::function<void()> VoidCallback;

	class WindowImpl {

	public:
		WindowImpl() = default;
		virtual ~WindowImpl() = default;

		virtual void init(int32_t width, int32_t height) = 0;

		virtual void waitForEvents() = 0;

		virtual bool shouldWindowClose() const = 0;
		virtual Int32Size getMonitorResolution() const = 0;
		virtual Int32Size getSize() const = 0;
		virtual Int32Size getFramebufferSize() const = 0;
		virtual std::vector<int32_t> getFrameSize() const = 0;
		virtual Int32Pos getPosition() const = 0;
		virtual const std::string& getTitle() const = 0;
		virtual bool isResizable() const = 0;
		virtual bool isFocused() const = 0;
		virtual DoublePos getMousePosition() const = 0;

		virtual void setSize(int32_t width, int32_t height) = 0;
		virtual void setPosition(int32_t x, int32_t y) = 0;
		virtual void setTitle(std::string title) = 0;
		virtual void setResizable(bool resizable) = 0;
		virtual void setMousePosition(double x, double y) = 0;
		virtual void setMouseHidden(bool enabled) = 0;
		virtual void setMouseCentered() = 0;

		virtual void onWindowResize(int32_t width, int32_t height);
		virtual void onWindowFocus(int focused);
		virtual void onFramebufferResize(int32_t width, int32_t height);
		virtual void onMouseScroll(double x, double y);
		virtual void onMouseMove(int32_t x, int32_t y);
		virtual void onMousePress(MouseButton button, bool pressed, int mods);
		virtual void onKeyPress(KeyButton button, KeyAction action);
		virtual void onMouseEnter();
		virtual void onMouseExit();

		void addWindowSizeCallback(SizeCallback callback);
		void addWindowFocusCallback(BoolCallback callback);
		void addFramebufferSizeCallback(SizeCallback callback);
		void addMousePosCallback(PosCallback callback);
		void addMouseScrollCallback(OffsetCallback callback);
		void addMouseButtonCallback(MouseButtonCallback callback);
		void addKeyButtonCallback(KeyPressCallback callback);
		void addMouseEnterCallback(VoidCallback callback);
		void addMouseExitCallback(VoidCallback callback);

		virtual MouseAction getMouseAction(int action) = 0;
		virtual MouseButton getMouseButton(int btn) = 0;
		virtual KeyAction getKeyAction(int action) = 0;
		virtual KeyButton getKeyButton(int btn) = 0;

	private:
		std::vector<SizeCallback> windowSizeCallbacks{};
		std::vector<BoolCallback> windowFocusCallbacks{};
		std::vector<SizeCallback> framebufferSizeCallbacks{};
		std::vector<PosCallback> mousePosCallbacks{};
		std::vector<OffsetCallback> mouseScrollCallbacks{};
		std::vector<MouseButtonCallback> mouseButtonCallbacks{};
		std::vector<KeyPressCallback> keyPressCallbacks{};
		std::vector<VoidCallback> mouseEnterCallbacks{};
		std::vector<VoidCallback> mouseExitCallbacks{};

	};

}