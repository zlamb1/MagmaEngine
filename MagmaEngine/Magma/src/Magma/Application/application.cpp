#include "application.h"

namespace Magma {

	int Application::run() {
        int exitStatus = EXIT_SUCCESS;

        try {
            initWindow();
            initVulkan();
            initMainLoop();
        }
        catch (const std::exception& error) {
            std::cout << error.what() << std::endl;
            exitStatus = EXIT_FAILURE;
        }

        return exitStatus;
	}

	void Application::initVulkan() {
        auto& vulkanImpl = dynamic_cast<VulkanImpl&>(*m_WindowImpl);
        m_RenderCore = std::make_shared<VulkanCore>(vulkanImpl);
	}

    void Application::initMainLoop() {
        while (!m_WindowImpl->shouldWindowClose()) {
            onNewFrame();
        }
    }

}