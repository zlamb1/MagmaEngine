#include "application.h"

namespace Magma {

	int Application::run() {
        int exit_status = EXIT_SUCCESS;
        try {
            initWindow();
            initVulkan();
            initMainLoop();
        }
        catch (const std::exception& error) {
            std::cout << error.what() << std::endl;
            exit_status = EXIT_FAILURE;
        }

        return exit_status;
	}

	void Application::initVulkan() {
        VulkanImpl& vulkanImpl = (VulkanImpl&)*windowImpl;
        renderCore = std::make_shared<VulkanAPI>(vulkanImpl);
	}

    void Application::initMainLoop() {
        while (!windowImpl->shouldWindowClose()) {
            onNewFrame();
        }
    }

}