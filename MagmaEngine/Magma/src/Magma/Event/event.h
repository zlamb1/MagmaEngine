#pragma once

#include <iostream>
#include <functional>
#include <map>
#include <typeinfo>

namespace Magma {

	struct Event {

	};

	struct WindowResizeEvent : public Event {
		WindowResizeEvent(int32_t width, int32_t height) : width{ width }, height{ height } {}
		int32_t width, height;
	};

	struct WindowFocusEvent : public Event {
		WindowFocusEvent(bool focused) : focused{ focused } {}
		bool focused;
	};

	struct MouseButtonEvent : public Event {
		MouseButtonEvent(MouseButton btn, bool pressed) : btn{ btn }, pressed{ pressed } {}
		MouseButton btn;
		bool pressed;
	};

	struct MouseMoveEvent : public Event {
		MouseMoveEvent(double x, double y) : x{ x }, y{ y } {};
		double x, y;
	};


	struct MouseScrollEvent : public Event {
		MouseScrollEvent(double y) : y{ y } {}
		double y; 
	};

	struct KeyPressEvent : public Event {
		KeyPressEvent(KeyButton btn, KeyAction action) : btn{ btn }, action{ action } {}
		KeyButton btn;
		KeyAction action;
	};

	class EventHandler {

	public:
		template<typename T>
		void dispatch(const Event& _event) {
			onEvent((const T&)_event);
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
			handlers[typeid(T).name()].push_back(handler);
		}

		template<typename T>
		void unsubscribe(EventHandler* handler) {
			auto& vector = handlers[typeid(T).name()];
			auto found = std::find(vector.begin(), vector.end(), handler);
			if (found != vector.end()) {
				vector.erase(found);
			}
		}

		template<typename T>
		void dispatch(const T& _event) const {
			auto name = typeid(T).name();
			if (handlers.find(name) == handlers.end())
				return;
			auto& _handlers = handlers.at(name);
			for (auto handler : _handlers) {
				handler->dispatch<T>(_event);
			}
		}
			
	private:
		std::map<const char*, std::vector<EventHandler*>> handlers{};

	};

}