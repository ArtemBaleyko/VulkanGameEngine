#pragma once

#include <memory>
#include <vector>

#include "Device.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Window.h"

namespace vge {
class Application {
public:
    Application();
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void run();

private:
    void loadGameObjects();

private:
    Window _window{"Vulkan Game Engine", 800, 600};
    Device _device{_window};
    Renderer _renderer{_window, _device};

    std::vector<GameObject> _gameObjects;
};
}  // namespace vge