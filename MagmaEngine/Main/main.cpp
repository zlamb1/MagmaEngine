#include "application.h"

int main() {
    Magma::Application& app = Magma::Application::instance();
    return app.run();
}