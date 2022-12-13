#pragma once

#include <iostream>
#include <functional>
#include <map>
#include <typeinfo>

namespace Magma {

	struct Event {

	};

	struct WindowResizeEvent : Event {
		WindowResizeEvent(int32_t width, int32_t height) : m_Width{ width }, m_Height{ height } {}
		int32_t m_Width, m_Height;
	};

	struct WindowFocusEvent : Event {
		WindowFocusEvent(bool focused) : m_Focused{ focused } {}
		bool m_Focused;
	};

	struct MouseButtonEvent : Event {
		MouseButtonEvent(MouseButton button, bool pressed) : m_Button{ button }, m_Pressed{ pressed } {}
		MouseButton m_Button;
		bool m_Pressed;
	};

	struct MouseMoveEvent : Event {
		MouseMoveEvent(double x, double y) : m_X{ x }, m_Y{ y } {}
		double m_X, m_Y;
	};


	struct MouseScrollEvent : Event {
		MouseScrollEvent(double y) : m_Y{ y } {}
		double m_Y; 
	};

	struct KeyPressEvent : Event {
		KeyPressEvent(KeyButton button, KeyAction action) : m_Button{ button }, m_Action{ action } {}
		KeyButton m_Button;
		KeyAction m_Action;
	};

	class EventHandler {

	public:
		template<typename T>
		void dispatch(const Event& _event) {
			onEvent(static_cast<const T&>(_event));
		}

		virtual void onEvent(const WindowResizeEvent& _event) {}
		virtual void onEvent(const WindowFocusEvent& _event) {}
		virtual void onEvent(const MouseButtonEvent& _event) {}
		virtual void onEvent(const MouseMoveEvent& _event) {}
		virtual void onEvent(const MouseScrollEvent& _event) {}
		virtual void onEvent(const KeyPressEvent& _event) {}

	};

	class EventDispatcher {

	public:
		template<typename T>
		void subscribe(EventHandler* handler) {
			m_Handlers[typeid(T).name()].push_back(handler);
		}

		template<typename T>
		void unsubscribe(EventHandler* handler) {
			auto& vector = m_Handlers[typeid(T).name()];
			const auto found = std::ranges::find(vector.begin(), 
				vector.end(), handler);
			if (found != vector.end()) {
				vector.erase(found);
			}
		}

		template<typename T>
		void dispatch(const T& _event) const {
			const auto name = typeid(T).name();
			if (!m_Handlers.contains(name)) {
				return;
			}
			auto& handlers = m_Handlers.at(name);
			for (auto handler : handlers) {
				handler->dispatch<T>(_event);
			}
		}
			
	private:
		std::map<const char*, std::vector<EventHandler*>> m_Handlers{};

	};

}