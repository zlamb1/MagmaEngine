#pragma once

#include "Magma/Vulkan/vulkan_api.h"

#include "Magma/Window/vulkan_impl.h"
#include "Magma/Window/window_input.h"

namespace Magma {

	class Application {

	public:
		Application() = default;
		int run();

	protected:
		virtual void initWindow() = 0; 
		virtual void initVulkan();

		virtual void onNewFrame() = 0;

	protected:
		std::shared_ptr<WindowImpl> windowImpl; 
		std::shared_ptr<WindowInput> input;
		std::shared_ptr<VulkanAPI> renderCore;

	private:
		void initMainLoop();

	};

}