#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace vge {
class Window {
public:
    Window(const std::string& name, uint32_t width, uint32_t height);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    inline bool shouldClose() const { return glfwWindowShouldClose(_window); }
    inline VkExtent2D getExtent() const { return {_width, _height}; }

    void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

private:
    void initWindow();

private:
    GLFWwindow* _window;
    uint32_t _width;
    uint32_t _height;

    std::string _name;
};
}  // namespace vge