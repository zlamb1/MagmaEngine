#include "application.h"

// Public

Application& Application::instance() {
    static Application app;
    return app;
}

int Application::run() {
    initWindow();
    initVulkan();
    mainLoop();

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
}

void Application::initVulkan() {
    vulkanAPI = std::make_unique<VulkanAPI>(*window);
    vulkanAPI->init();
}

void Application::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        //vulkanAPI->newFrame();

        glfwPollEvents();
    }
}