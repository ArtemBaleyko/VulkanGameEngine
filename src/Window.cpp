#include "Window.h"

namespace vge {
Window::Window(const std::string& name, uint32_t width, uint32_t height)
    : _name(name)
    , _width(width)
    , _height(height) {
    initWindow();
}

Window::~Window() {
    glfwDestroyWindow(_window);
    glfwTerminate();
}

void Window::initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    _window = glfwCreateWindow(_width, _height, _name.c_str(), nullptr, nullptr);
}
}  // namespace vge