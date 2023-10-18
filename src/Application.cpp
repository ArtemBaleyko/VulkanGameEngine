#include "Application.h"

namespace vge {
void Application::run() {
    while (!_window.shouldClose()) {
        glfwPollEvents();
    }
}
}  // namespace vge