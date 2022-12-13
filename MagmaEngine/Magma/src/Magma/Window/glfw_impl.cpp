#include "glfw_impl.h"

#define MakeSize(x, y) std::make_pair<int32_t, int32_t>(static_cast<int32_t>(x), static_cast<int32_t>(y))
#define MakePos(x, y) std::make_pair<int32_t, int32_t>(static_cast<int32_t>(x), static_cast<int32_t>(y))

namespace Magma {

	GLFWImpl::~GLFWImpl() {
		glfwDestroyWindow(m_Window);
	}

	void GLFWImpl::init(const int32_t width, const int32_t height) {
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		m_Window = glfwCreateWindow(width, height, "", nullptr, nullptr);

		glfwSetWindowUserPointer(m_Window, this);

		glfwSetWindowSizeCallback(m_Window, glfwOnWindowResize);
		glfwSetWindowFocusCallback(m_Window, glfwOnWindowFocus);
		glfwSetFramebufferSizeCallback(m_Window, glfwOnFramebufferResize);
		glfwSetCursorPosCallback(m_Window, glfwOnMouseMove);
		glfwSetScrollCallback(m_Window, glfwOnMouseScroll);
		glfwSetMouseButtonCallback(m_Window, glfwOnMousePress);
		glfwSetCursorEnterCallback(m_Window, glfwOnMouseEnter);
		glfwSetKeyCallback(m_Window, glfwOnKeyPress);
	}

	void GLFWImpl::waitForEvents() {
		glfwWaitEvents();
	}

	bool GLFWImpl::shouldWindowClose() const {
		return glfwWindowShouldClose(m_Window);
	}

	std::pair<int32_t, int32_t> GLFWImpl::getMonitorResolution() const {
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		return MakeSize(mode->width, mode->height);
	}

	std::pair<int32_t, int32_t> GLFWImpl::getSize() const {
		int32_t width, height;
		glfwGetWindowSize(m_Window, &width, &height);
		return MakeSize(width, height);
	}

	Int32Size GLFWImpl::getFramebufferSize() const {
		int32_t width, height;
		glfwGetFramebufferSize(m_Window, &width, &height);
		return MakeSize(width, height);
	}

	std::vector<int32_t> GLFWImpl::getFrameSize() const {
		int32_t l, t, r, b;
		glfwGetWindowFrameSize(m_Window, &l, &t, &r, &b);
		std::vector<int32_t> sizes{ l, t, r, b };
		return sizes; 
	}

	Int32Pos GLFWImpl::getPosition() const {
		int x, y;
		glfwGetWindowPos(m_Window, &x, &y);
		return MakePos(x, y);
	}

	const std::string& GLFWImpl::getTitle() const {
		return m_Title;
	}

	DoublePos GLFWImpl::getMousePosition() const {
		double x, y;
		glfwGetCursorPos(m_Window, &x, &y);
		return std::make_pair<double, double>((double)x, (double)y);
	}

	bool GLFWImpl::isResizable() const {
		return glfwGetWindowAttrib(m_Window, GLFW_RESIZABLE);
	}

	bool GLFWImpl::isFocused() const {
		return glfwGetWindowAttrib(m_Window, GLFW_FOCUSED);
	}

	void GLFWImpl::setSize(const int32_t width, const int32_t height) {
		glfwSetWindowSize(m_Window, width, height);
	}

	void GLFWImpl::setPosition(const int32_t x, const int32_t y) {
		glfwSetWindowPos(m_Window, x, y);
	}

	void GLFWImpl::setTitle(const std::string title) {
		glfwSetWindowTitle(m_Window, title.c_str());
		m_Title = title;
	}

	void GLFWImpl::setResizable(bool resizable) {
		glfwSetWindowAttrib(m_Window, GLFW_RESIZABLE, resizable);
	}

	void GLFWImpl::setMousePosition(double x, double y) {
		glfwSetCursorPos(m_Window, x, y);
	}

	void GLFWImpl::setMouseHidden(bool hidden) {
		if (hidden) {
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else {
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}

	void GLFWImpl::setMouseCentered() {
		auto [width, height] = getSize();
		glfwSetCursorPos(m_Window, width / 2.0, height / 2.0);
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
		return glfwCreateWindowSurface(vkInstance, m_Window, nullptr, &vkSurfaceKHR);
	}

	GLFWImpl* GLFWImpl::getUserPtrFromWindow(GLFWwindow* window) {
		return static_cast<GLFWImpl*>(glfwGetWindowUserPointer(window));
	}

	void GLFWImpl::glfwOnWindowResize(GLFWwindow* window, int32_t width,
		int32_t height) {
		getUserPtrFromWindow(window)->onWindowResize(width, height);
	}

	void GLFWImpl::glfwOnWindowFocus(GLFWwindow* window, int focused) {
		getUserPtrFromWindow(window)->onWindowFocus(focused);
	}

	void GLFWImpl::glfwOnFramebufferResize(GLFWwindow* window, int32_t width,
		int32_t height) {
		getUserPtrFromWindow(window)->onFramebufferResize(width, height);
	}

	void GLFWImpl::glfwOnMouseMove(GLFWwindow* window, const double x, const double y) {
		getUserPtrFromWindow(window)->onMouseMove(static_cast<int32_t>(x), static_cast<int32_t>(y));
	}

	void GLFWImpl::glfwOnMouseScroll(GLFWwindow* window, double x, double y) {
		getUserPtrFromWindow(window)->onMouseScroll(x, y);
	}

	void GLFWImpl::glfwOnMousePress(GLFWwindow* window, int button, int action, int mods) {
		const auto windowPtr = getUserPtrFromWindow(window);
		const int pressed = std::min(1, std::max(0, 
		static_cast<int>(windowPtr->getMouseAction(action))));
		windowPtr->onMousePress(windowPtr->getMouseButton(button), pressed, mods);
	}

	void GLFWImpl::glfwOnMouseEnter(GLFWwindow* window, int entered) {
		const auto windowPtr = getUserPtrFromWindow(window);
		if (entered) {
			windowPtr->onMouseEnter();
		}
		else {
			windowPtr->onMouseExit();
		}
	}

	void GLFWImpl::glfwOnKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods) {
		const auto windowPtr = getUserPtrFromWindow(window);
		windowPtr->onKeyPress(windowPtr->getKeyButton(key), windowPtr->getKeyAction(action));
	}

}