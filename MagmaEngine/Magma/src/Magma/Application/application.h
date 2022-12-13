#pragma once

#include "Magma/Vulkan/vulkan_core.h"

#include "Magma/Window/vulkan_impl.h"
#include "Magma/Window/window_input.h"

namespace Magma {

	class Application {

	public:
		Application() = default;
		virtual ~Application() = default; 
		int run();

	protected:
		virtual void initWindow() = 0; 
		virtual void initVulkan();

		virtual void onNewFrame() = 0;

	protected:
		std::shared_ptr<WindowImpl> m_WindowImpl; 
		std::shared_ptr<WindowInput> m_Input;
		std::shared_ptr<VulkanCore> m_RenderCore;

	private:
		void initMainLoop();

	};

}