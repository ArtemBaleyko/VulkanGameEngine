#include "Window.h"

#include <stdexcept>

namespace vge {
Window::Window(const std::string& name, uint32_t width, uint32_t height)
    : _name(name)
    , _width(width)
    , _height(height)
    , _isFramebufferResized(false) {
    initWindow();
}

Window::~Window() {
    glfwDestroyWindow(_window);
    glfwTerminate();
}

void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
    if (glfwCreateWindowSurface(instance, _window, nullptr, surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface");
    }
}

void Window::frameBufferResizedCallback(GLFWwindow* window, int width, int height) {
    auto vgeWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    vgeWindow->_isFramebufferResized = true;
    vgeWindow->_width = width;
    vgeWindow->_height = height;
}

void Window::initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    _window = glfwCreateWindow(_width, _height, _name.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(_window, this);
    glfwSetFramebufferSizeCallback(_window, frameBufferResizedCallback);
}
}  // namespace vge