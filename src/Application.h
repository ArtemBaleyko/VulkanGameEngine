#pragma once

#include "Device.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Window.h"

#include <memory>
#include <vector>

namespace vge {
class Application {
public:
    static constexpr uint32_t WIDTH = 800;
    static constexpr uint32_t HEIGHT = 600;

    Application();
    ~Application();

    Application(const Application&) = delete;
    Application &operator=(const Application&) = delete;

    void run();

private:
    void loadGameObjects();

    Window _window{WIDTH, HEIGHT, "Vulkan Game Engine"};
    Device _device{_window};
    Renderer _renderer{_window, _device};

    std::vector<GameObject> _gameObjects;
};
}  // namespace vge
