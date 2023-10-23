#include "Application.h"

#include "KeyboardMovementController.h"
#include "Camera.h"
#include "RenderSystem.h"
#include "Buffer.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <cassert>
#include <chrono>
#include <stdexcept>

namespace vge {

struct GlobalUbo {
    glm::mat4 projectionView{1.0f};
    glm::vec3 lightDirection = glm::normalize(glm::vec3(-1.0f, -3.0f, -1.0f));
};

Application::Application() { loadGameObjects(); }

Application::~Application() {}

void Application::run() {
    Buffer uniformBuffer{_device,
                         sizeof(GlobalUbo),
                         SwapChain::MAX_FRAMES_IN_FLIGHT,
                         VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                         _device.properties.limits.minUniformBufferOffsetAlignment};


    uniformBuffer.map();

    RenderSystem renderSystem{_device, _renderer.getSwapChainRenderPass()};
    Camera camera{};

    auto viewerObject = GameObject::createGameObject();
    KeyBoardMovementController cameraController{};

    auto currentTime = std::chrono::high_resolution_clock::now();

    while (!_window.shouldClose()) {
        glfwPollEvents();

        auto newTime = std::chrono::high_resolution_clock::now();
        auto frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        cameraController.moveInPlaneXZ(_window.getGLFWWindow(), frameTime, viewerObject);
        camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

        float aspect = _renderer.getAspectRatio();
        camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);

        if (auto commandBuffer = _renderer.beginFrame()) {
            int frameIndex = _renderer.getFrameIndex();
            FrameInfo frameInfo{frameIndex, frameTime, commandBuffer, camera};
            

            GlobalUbo ubo{};
            ubo.projectionView = camera.getProjectionViewMatrix();
            uniformBuffer.writeToIndex(&ubo, frameIndex);
            uniformBuffer.flushIndex(frameIndex);

            
            _renderer.beginSwapChainRenderPass(commandBuffer);

            renderSystem.renderGameObjects(frameInfo, _gameObjects);

            _renderer.endSwapChainRenderPass(commandBuffer);
            _renderer.endFrame();
        }
    }

    vkDeviceWaitIdle(_device.getVkDevice());
}

void Application::loadGameObjects() {
    std::shared_ptr<Model> model = Model::createModelFromFile(_device, "../models/smooth_vase.obj");
    auto cube = GameObject::createGameObject();
    cube.model = model;
    cube.transform.translation = {.0f, .0f, 0.5f};
    cube.transform.scale = {.8f, .8f, .8f};
    _gameObjects.push_back(std::move(cube));
}

}  // namespace vge
