#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <unordered_map>

#include "Model.h"

namespace vge {

struct TransformComponent {
    glm::vec3 translation{};
    glm::vec3 scale{1.f, 1.f, 1.f};
    glm::vec3 rotation{};

    glm::mat4 mat4() const;
    glm::mat3 normalMatrix() const;
};

struct PointLightComponent {
    float lightIntensity = 1.0f;
};

class GameObject {
public:
    using id_t = unsigned int;
    using Map = std::unordered_map<id_t, GameObject>;

    static GameObject createGameObject();
    static GameObject createPointLight(float intensity = 10.0f, float radius = 0.1f, const glm::vec3& color = {1.0f, 1.0f, 1.0f});

    GameObject(const GameObject &) = delete;
    GameObject &operator=(const GameObject &) = delete;
    GameObject(GameObject &&) = default;
    GameObject &operator=(GameObject &&) = default;

    inline id_t getId() const { return _id; }

    std::shared_ptr<Model> model{};
    glm::vec3 color{};
    TransformComponent transform{};

    std::unique_ptr<PointLightComponent> pointLight = nullptr;

private:
    GameObject(id_t objId)
        : _id{objId} {}

    id_t _id;
};
}  // namespace vge
