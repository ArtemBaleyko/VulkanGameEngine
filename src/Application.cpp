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
#include <algorithm>

namespace vge {

struct GlobalUbo {
    glm::mat4 projectionView{1.0f};
    glm::vec4 ambientLightColor{1.0f, 1.0f, 1.0f, 0.2f};
    glm::vec3 lightPosition{-1.0f};
    alignas(16) glm::vec4 lightColor{1.0f};
};

Application::Application() { 
    _globalPool = DescriptorPool::Builder(_device)
                      .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
                      .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
                      .build();
    loadGameObjects(); 
}

Application::~Application() {}

void Application::run() {
    std::vector<std::unique_ptr<Buffer>> uniformBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < uniformBuffers.size(); i++) {
        uniformBuffers[i] = std::make_unique<Buffer>(_device,
                                                     sizeof(GlobalUbo),
                                                     SwapChain::MAX_FRAMES_IN_FLIGHT,
                                                     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

        uniformBuffers[i]->map();
    }

    auto globalSetLayout = DescriptorSetLayout::Builder(_device)
                               .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                               .build();

    std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < globalDescriptorSets.size(); i++) {
        auto bufferInfo = uniformBuffers[i]->descriptorInfo();
        DescriptorWriter(*globalSetLayout, *_globalPool)
            .writeBuffer(0, &bufferInfo)
            .build(globalDescriptorSets[i]);
    }

    RenderSystem renderSystem{
        _device, _renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
    Camera camera{};

    auto viewerObject = GameObject::createGameObject();
    viewerObject.transform.translation.z = -2.5f;
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
        camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

        if (auto commandBuffer = _renderer.beginFrame()) {
            int frameIndex = _renderer.getFrameIndex();
            FrameInfo frameInfo{
                frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex], _gameObjects};
            

            GlobalUbo ubo{};
            ubo.projectionView = camera.getProjectionViewMatrix();

            uniformBuffers[frameIndex]->writeToBuffer(&ubo);
            uniformBuffers[frameIndex]->flush();
            
            _renderer.beginSwapChainRenderPass(commandBuffer);

            renderSystem.renderGameObjects(frameInfo);

            _renderer.endSwapChainRenderPass(commandBuffer);
            _renderer.endFrame();
        }
    }

    vkDeviceWaitIdle(_device.getVkDevice());
}

void Application::loadGameObjects() {
    {
        std::shared_ptr<Model> model = Model::createModelFromFile(_device, "../models/smooth_vase.obj");
        auto obj = GameObject::createGameObject();
        obj.model = model;
        obj.transform.translation = {-0.5f, 0.5f, 0.0f};
        _gameObjects.emplace(obj.getId(), std::move(obj));
    }
    {
        std::shared_ptr<Model> model = Model::createModelFromFile(_device, "../models/smooth_vase.obj");
        auto obj = GameObject::createGameObject();
        obj.model = model;
        obj.transform.translation = {0.5f, 0.5f, 2.0f};
        _gameObjects.emplace(obj.getId(), std::move(obj));
    }
    {
        std::shared_ptr<Model> model = Model::createModelFromFile(_device, "../models/quad.obj");
        auto obj = GameObject::createGameObject();
        obj.model = model;
        obj.transform.scale = {3.0f, 1.0f, 3.0f};
        obj.transform.translation = {0.0f, 0.5f, 0.0f};
        _gameObjects.emplace(obj.getId(), std::move(obj));
    }
}

}  // namespace vge
