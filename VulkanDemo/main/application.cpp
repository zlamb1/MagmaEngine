#include "application.h"

// Public

Application& Application::instance() {
    static Application app;
    return app;
}

int Application::run() {

    int exit_status = EXIT_SUCCESS;

    try {
        initWindow();
        initVulkan();
        mainLoop();
    }
    catch (const std::exception& error) {
        std::cout << error.what() << std::endl;
        exit_status = EXIT_FAILURE;
    }   

    system("pause");

    return EXIT_SUCCESS;
}

// Private

Application::~Application() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Application::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    const GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    glfwSetWindowPos(window, mode->width - WIDTH, 30);
}

void Application::initVulkan() {
    vulkanAPI = std::make_unique<VulkanAPI>(*window);
    vulkanAPI->init();
}

void Application::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        vulkanAPI->onNewFrame();

        glfwPollEvents();
    }
}