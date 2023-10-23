#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
namespace vge {

class Window {
public:
    Window(int width, int height, std::string name);
    ~Window();

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    inline bool shouldClose() const { return glfwWindowShouldClose(_window); }
    inline VkExtent2D getExtent() const { return {static_cast<uint32_t>(_width), static_cast<uint32_t>(_height)}; }
    inline bool wasResized() const { return _isFramebufferResized; }
    inline GLFWwindow *getGLFWWindow() const { return _window; }

    inline void resetWindowResizedFlag() { _isFramebufferResized = false; }

    void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

private:
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    void initWindow();

private:
    int _width;
    int _height;
    bool _isFramebufferResized;

    std::string _name;
    GLFWwindow* _window;
};
}  // namespace vge
