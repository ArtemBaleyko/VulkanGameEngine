#include "Window.h"

#include <stdexcept>

namespace vge {

Window::Window(int width, int height, std::string name)
    : _width{width}
    , _height{height}
    , _isFramebufferResized{false}
    , _name{name} {
    initWindow();
}

Window::~Window() {
    glfwDestroyWindow(_window);
    glfwTerminate();
}

void Window::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    _window = glfwCreateWindow(_width, _height, _name.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(_window, this);
    glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);
}

void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
    if (glfwCreateWindowSurface(instance, _window, nullptr, surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to craete window surface");
    }
}

void Window::framebufferResizeCallback(GLFWwindow *_window, int _width, int _height) {
    auto window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(_window));
    window->_isFramebufferResized = true;
    window->_width = _width;
    window->_height = _height;
}

}  // namespace vge
