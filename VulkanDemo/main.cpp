#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "application.h"

int main() {
    Application& app = Application::instance();
    return app.run();
}