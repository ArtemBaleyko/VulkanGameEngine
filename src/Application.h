#pragma once

#include "Window.h"

namespace vge {
class Application {
public:
    void run();

private:
    Window _window{"Vulkan Game Engine", 800, 600};
};
}  // namespace vge