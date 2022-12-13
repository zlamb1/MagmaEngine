#include "glfw_impl.h"

#define MakeSize(x, y) std::make_pair<int32_t, int32_t>((int32_t)x, (int32_t)y)
#define MakePos(x, y) std::make_pair<int32_t, int32_t>((int32_t)x, (int32_t)y)

namespace Magma {

	GLFWImpl::~GLFWImpl() {
		glfwDestroyWindow(window);
	}

	void GLFWImpl::init(int32_t width, int32_t height) {
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		window = glfwCreateWindow(width, height, "", nullptr, nullptr);

		glfwSetWindowUserPointer(window, this);

		glfwSetWindowSizeCallback(window, glfwOnWindowResize);
		glfwSetWindowFocusCallback(window, glfwOnWindowFocus);
		glfwSetFramebufferSizeCallback(window, glfwOnFramebufferResize);
		glfwSetCursorPosCallback(window, glfwOnMouseMove);
		glfwSetScrollCallback(window, glfwOnMouseScroll);
		glfwSetMouseButtonCallback(window, glfwOnMousePress);
		glfwSetCursorEnterCallback(window, glfwOnMouseEnter);
		glfwSetKeyCallback(window, glfwOnKeyPress);
	}

	void GLFWImpl::waitForEvents() {
		glfwWaitEvents();
	}

	bool GLFWImpl::shouldWindowClose() const {
		return glfwWindowShouldClose(window);
	}

	std::pair<int32_t, int32_t> GLFWImpl::getMonitorResolution() const {
		const GLFWmonitor* primary = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		return MakeSize(mode->width, mode->height);
	}

	std::pair<int32_t, int32_t> GLFWImpl::getSize() const {
		int32_t width, height;
		glfwGetWindowSize(window, &width, &height);
		return MakeSize(width, height);
	}

	Int32Size GLFWImpl::getFramebufferSize() const {
		int32_t width, height;
		glfwGetFramebufferSize(window, &width, &height);
		return MakeSize(width, height);
	}

	std::vector<int32_t> GLFWImpl::getFrameSize() const {
		int32_t l, t, r, b;
		glfwGetWindowFrameSize(window, &l, &t, &r, &b);
		std::vector<int32_t> sizes{ l, t, r, b };
		return sizes; 
	}

	Int32Pos GLFWImpl::getPosition() const {
		int x, y;
		glfwGetWindowPos(window, &x, &y);
		return MakePos(x, y);
	}

	const std::string& GLFWImpl::getTitle() const {
		return title;
	}

	DoublePos GLFWImpl::getMousePosition() const {
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		return std::make_pair<double, double>((double)x, (double)y);
	}

	bool GLFWImpl::isResizable() const {
		return glfwGetWindowAttrib(window, GLFW_RESIZABLE);
	}

	bool GLFWImpl::isFocused() const {
		return glfwGetWindowAttrib(window, GLFW_FOCUSED);
	}

	void GLFWImpl::setSize(int32_t width, int32_t height) {
		glfwSetWindowSize(window, width, height);
	}

	void GLFWImpl::setPosition(int32_t x, int32_t y) {
		glfwSetWindowPos(window, x, y);
	}

	void GLFWImpl::setTitle(std::string title) {
		glfwSetWindowTitle(window, title.c_str());
		this->title = title;
	}

	void GLFWImpl::setResizable(bool resizable) {
		glfwSetWindowAttrib(window, GLFW_RESIZABLE, resizable);
	}

	void GLFWImpl::setMousePosition(double x, double y) {
		glfwSetCursorPos(window, x, y);
	}

	void GLFWImpl::setMouseHidden(bool hidden) {
		if (hidden) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}

	void GLFWImpl::setMouseCentered() {
		auto windowSize = getSize();
		glfwSetCursorPos(window, windowSize.first / 2.0, windowSize.second / 2.0);
	}

	MouseAction GLFWImpl::getMouseAction(int action) {
		switch (action) {
		case GLFW_RELEASE:
			return MouseAction::RELEASE;
		case GLFW_PRESS:
			return MouseAction::PRESS;
		default:
			return MouseAction::UNKNOWN;
		}
	}

	MouseButton GLFWImpl::getMouseButton(int btn) {
		switch (btn) {
		case GLFW_MOUSE_BUTTON_LEFT:
			return MouseButton::LEFT;
		case GLFW_MOUSE_BUTTON_RIGHT:
			return MouseButton::RIGHT;
		default:
			return MouseButton::UNKNOWN;
		}
	}

	KeyAction GLFWImpl::getKeyAction(int action) {
		switch (action) {
			case GLFW_RELEASE:
				return KeyAction::RELEASE;
			case GLFW_PRESS:
				return KeyAction::PRESS;
			case GLFW_REPEAT:
				return KeyAction::REPEAT;
			default:
				return KeyAction::UNKNOWN;
		}
	}

	KeyButton GLFWImpl::getKeyButton(int btn) {
		switch (btn) {
			case GLFW_KEY_TAB:
				return KeyButton::TAB;
			case GLFW_KEY_Q:
				return KeyButton::Q;
			case GLFW_KEY_W:
				return KeyButton::W;
			case GLFW_KEY_E:
				return KeyButton::E;
			case GLFW_KEY_R:
				return KeyButton::R;
			case GLFW_KEY_T:
				return KeyButton::T;
			case GLFW_KEY_Y:
				return KeyButton::Y;
			case GLFW_KEY_U:
				return KeyButton::U;
			case GLFW_KEY_I:
				return KeyButton::I;
			case GLFW_KEY_O:
				return KeyButton::O;
			case GLFW_KEY_P:
				return KeyButton::P;
			case GLFW_KEY_LEFT_BRACKET:
				return KeyButton::LEFT_BRACKET;
			case GLFW_KEY_RIGHT_BRACKET:
				return KeyButton::RIGHT_BRACKET;
			case GLFW_KEY_BACKSLASH:
				return KeyButton::BACKSLASH;
			case GLFW_KEY_CAPS_LOCK:
				return KeyButton::CAPS_LOCK;
			case GLFW_KEY_A:
				return KeyButton::A;
			case GLFW_KEY_S:
				return KeyButton::S;
			case GLFW_KEY_D:
				return KeyButton::D;
			case GLFW_KEY_F:
				return KeyButton::F;
			case GLFW_KEY_G:
				return KeyButton::G;
			case GLFW_KEY_H:
				return KeyButton::H;
			case GLFW_KEY_J:
				return KeyButton::J;
			case GLFW_KEY_K:
				return KeyButton::K;
			case GLFW_KEY_L:
				return KeyButton::L;
			case GLFW_KEY_SEMICOLON:
				return KeyButton::SEMICOLON;
			case GLFW_KEY_APOSTROPHE:
				return KeyButton::APOSTROPHE;
			case GLFW_KEY_ENTER:
				return KeyButton::ENTER;
			case GLFW_KEY_LEFT_SHIFT:
				return KeyButton::LEFT_SHIFT;
			case GLFW_KEY_Z:
				return KeyButton::Z;
			case GLFW_KEY_X:
				return KeyButton::X;
			case GLFW_KEY_C:
				return KeyButton::C;
			case GLFW_KEY_V:
				return KeyButton::V;
			case GLFW_KEY_B:
				return KeyButton::B;
			case GLFW_KEY_N:
				return KeyButton::N;
			case GLFW_KEY_M:
				return KeyButton::M;
			case GLFW_KEY_COMMA:
				return KeyButton::COMMA;
			case GLFW_KEY_PERIOD:
				return KeyButton::PERIOD;
			case GLFW_KEY_SLASH:
				return KeyButton::SLASH;
			case GLFW_KEY_LEFT_CONTROL:
				return KeyButton::LEFT_CONTROL;
			case GLFW_KEY_LEFT_ALT:
				return KeyButton::LEFT_ALT;
			case GLFW_KEY_SPACE:
				return KeyButton::SPACE;
			case GLFW_KEY_RIGHT_ALT:
				return KeyButton::RIGHT_ALT;
			case GLFW_KEY_RIGHT_CONTROL:
				return KeyButton::RIGHT_CONTROL;
			default:
				return KeyButton::UNKNOWN;
		}
	}

	VkResult GLFWImpl::getSurfaceKHR(VkInstance& vkInstance, VkSurfaceKHR& vkSurfaceKHR) {
		return glfwCreateWindowSurface(vkInstance, window, nullptr, &vkSurfaceKHR);
	}

	GLFWImpl* GLFWImpl::getUserPtrFromWindow(GLFWwindow* window) {
		return (GLFWImpl*)glfwGetWindowUserPointer(window);
	}

	void GLFWImpl::glfwOnWindowResize(GLFWwindow* window, int32_t width,
		int32_t height) {
		auto windowPtr = getUserPtrFromWindow(window);
		windowPtr->onWindowResize(width, height);
	}

	void GLFWImpl::glfwOnWindowFocus(GLFWwindow* window, int focused) {
		auto windowPtr = getUserPtrFromWindow(window);
		windowPtr->onWindowFocus(focused);
	}

	void GLFWImpl::glfwOnFramebufferResize(GLFWwindow* window, int32_t width,
		int32_t height) {
		auto windowPtr = getUserPtrFromWindow(window);
		windowPtr->onFramebufferResize(width, height);
	}

	void GLFWImpl::glfwOnMouseMove(GLFWwindow* window, double x, double y) {
		auto windowPtr = getUserPtrFromWindow(window);
		windowPtr->onMouseMove(x, y);
	}

	void GLFWImpl::glfwOnMouseScroll(GLFWwindow* window, double x, double y) {
		auto windowPtr = getUserPtrFromWindow(window);
		windowPtr->onMouseScroll(x, y);
	}

	void GLFWImpl::glfwOnMousePress(GLFWwindow* window, int btn, int action, int mods) {
		auto windowPtr = getUserPtrFromWindow(window);
		int pressed = std::min(1, std::max(0, (int)windowPtr->getMouseAction(action)));
		windowPtr->onMousePress(windowPtr->getMouseButton(btn), pressed, mods);
	}

	void GLFWImpl::glfwOnMouseEnter(GLFWwindow* window, int entered) {
		auto windowPtr = getUserPtrFromWindow(window);
		if (entered) {
			windowPtr->onMouseEnter();
		}
		else {
			windowPtr->onMouseExit();
		}
	}

	void GLFWImpl::glfwOnKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods) {
		auto windowPtr = getUserPtrFromWindow(window);
		windowPtr->onKeyPress(windowPtr->getKeyButton(key), windowPtr->getKeyAction(action));
	}

}