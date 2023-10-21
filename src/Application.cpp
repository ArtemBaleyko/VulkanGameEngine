#include "Application.h"

#include <array>
#include <stdexcept>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "RenderSystem.h"

namespace vge {

Application::Application() { loadGameObjects(); }

Application::~Application() {}

void Application::run() {
    RenderSystem renderSystem{_device, _renderer.getSwapChainRenderPass()};

    while (!_window.shouldClose()) {
        glfwPollEvents();

        if (auto commandBuffer = _renderer.beginFrame()) {
            _renderer.beginSwapChainRenderPass(commandBuffer);
            renderSystem.renderGameObjects(commandBuffer, _gameObjects);
            _renderer.endSwapChainRenderPass(commandBuffer);
            _renderer.endFrame();
        }
    }

    vkDeviceWaitIdle(_device.getVkDevice());
}

void Application::loadGameObjects() {
    std::vector<Model::Vertex> vertices = {
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f},  {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    };

    auto model = std::make_shared<Model>(_device, vertices);
    auto triangle = GameObject::createGameObject();
    triangle.model = model;
    triangle.color = {0.1f, 0.8f, 0.1f};
    triangle.transform2D.translation.x = 0.2f;
    triangle.transform2D.rotation = 0.25f * glm::two_pi<float>();

    _gameObjects.push_back(std::move(triangle));
}

}  // namespace vge