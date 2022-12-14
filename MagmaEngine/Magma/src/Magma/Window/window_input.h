#pragma once

#include "Magma/Window/window.h"
#include "Magma/Event/event.h"

namespace Magma {

	class WindowInput {

	public:
		WindowInput(WindowImpl& windowImpl);

		void registerCallbacks();

		bool isMouseBtnPressed(MouseButton btn);
		bool isKeyPressed(KeyButton btn);

		EventDispatcher& getEventDispatcher(); 
		WindowImpl& getWindowImpl() const;

	private:
		WindowImpl& windowImpl;
		EventDispatcher eventDispatcher{};

		std::map<MouseButton, bool> mButtons{};

		std::map<KeyButton, KeyAction> kButtons{};

	};
	
}