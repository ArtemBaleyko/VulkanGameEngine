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

std::unique_ptr<Model> createCubeModel(Device& device, glm::vec3 offset) {
    std::vector<Model::Vertex> vertices{

  // left face (white)
        {{-.5f, -.5f, -.5f},  {.9f, .9f, .9f}},
        {{-.5f, .5f, .5f},    {.9f, .9f, .9f}},
        {{-.5f, -.5f, .5f},   {.9f, .9f, .9f}},
        {{-.5f, -.5f, -.5f},  {.9f, .9f, .9f}},
        {{-.5f, .5f, -.5f},   {.9f, .9f, .9f}},
        {{-.5f, .5f, .5f},    {.9f, .9f, .9f}},

 // right face (yellow)
        {{.5f, -.5f, -.5f},   {.8f, .8f, .1f}},
        {{.5f, .5f, .5f},     {.8f, .8f, .1f}},
        {{.5f, -.5f, .5f},    {.8f, .8f, .1f}},
        {{.5f, -.5f, -.5f},   {.8f, .8f, .1f}},
        {{.5f, .5f, -.5f},    {.8f, .8f, .1f}},
        {{.5f, .5f, .5f},     {.8f, .8f, .1f}},

 // top face (orange, remember y axis points down)
        {{-.5f, -.5f, -.5f},  {.9f, .6f, .1f}},
        {{.5f, -.5f, .5f},    {.9f, .6f, .1f}},
        {{-.5f, -.5f, .5f},   {.9f, .6f, .1f}},
        {{-.5f, -.5f, -.5f},  {.9f, .6f, .1f}},
        {{.5f, -.5f, -.5f},   {.9f, .6f, .1f}},
        {{.5f, -.5f, .5f},    {.9f, .6f, .1f}},

 // bottom face (red)
        {{-.5f, .5f, -.5f},   {.8f, .1f, .1f}},
        {{.5f, .5f, .5f},     {.8f, .1f, .1f}},
        {{-.5f, .5f, .5f},    {.8f, .1f, .1f}},
        {{-.5f, .5f, -.5f},   {.8f, .1f, .1f}},
        {{.5f, .5f, -.5f},    {.8f, .1f, .1f}},
        {{.5f, .5f, .5f},     {.8f, .1f, .1f}},

 // nose face (blue)
        {{-.5f, -.5f, 0.5f},  {.1f, .1f, .8f}},
        {{.5f, .5f, 0.5f},    {.1f, .1f, .8f}},
        {{-.5f, .5f, 0.5f},   {.1f, .1f, .8f}},
        {{-.5f, -.5f, 0.5f},  {.1f, .1f, .8f}},
        {{.5f, -.5f, 0.5f},   {.1f, .1f, .8f}},
        {{.5f, .5f, 0.5f},    {.1f, .1f, .8f}},

 // tail face (green)
        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, .5f, -0.5f},   {.1f, .8f, .1f}},
        {{-.5f, .5f, -0.5f},  {.1f, .8f, .1f}},
        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, -.5f, -0.5f},  {.1f, .8f, .1f}},
        {{.5f, .5f, -0.5f},   {.1f, .8f, .1f}},
    };
    for (auto& v : vertices) {
        v.position += offset;
    }
    return std::make_unique<Model>(device, vertices);
}

void Application::loadGameObjects() {
    std::shared_ptr<Model> model = createCubeModel(_device, {0.0f, 0.0f, 0.0f});
    auto cube = GameObject::createGameObject();
    cube.model = model;
    cube.transform.translation = {0.0f, 0.0f, 0.5f};
    cube.transform.scale = {0.5f, 0.5f, 0.5f};

    _gameObjects.push_back(std::move(cube));
}

}  // namespace vge