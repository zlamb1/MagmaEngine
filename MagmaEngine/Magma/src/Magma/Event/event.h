#pragma once

#include <functional>
#include <map>
#include <typeinfo>

namespace Magma {

	struct Event {

	};

	struct MouseMoveEvent : public Event {

		MouseMoveEvent(double x, double y) : x{ x }, y{ y } {};
		double x, y;

	};

	class EventDispatcher {

	public:
		using EventObserver = std::function<void(const Event&)>;

		template<typename T>
		void subscribe(EventObserver observer) {
			observers[typeid(T).name()].push_back(observer);
		}

		template<typename T>
		void dispatch(const T& event) const {
			auto name = typeid(T).name();
			if (observers.find(name) == observers.end()) {
				return;
			}
			auto& _observers = observers.at(name);
			for (auto& observer : _observers) {
				observer(event);
			}
		}
			
	private:
		std::map<const char*, std::vector<EventObserver>> observers;

	};

}