#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <memory>

#include "Model.h"

namespace vge {

struct Transform2DComponent {
    glm::vec2 translation{};
    glm::vec2 scale{1.0f};
    float rotation;

    inline glm::mat2 mat2() const {
        const float s = glm::sin(rotation);
        const float c = glm::cos(rotation);

        glm::mat2 rotationMat = {
            {c,  s},
            {-s, c}
        };

        glm::mat2 scaleMat{
            {scale.x, 0.0f   },
            {0.0f,    scale.y}
        };
        return rotationMat * scaleMat;
    }
};

class GameObject {
public:
    using id_t = unsigned int;

    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;

    GameObject(GameObject&&) = default;
    GameObject& operator=(GameObject&&) = default;

    static GameObject createGameObject();

    inline id_t getId() const { return _id; }

public:
    glm::vec3 color;
    std::shared_ptr<Model> model;
    Transform2DComponent transform2D;

private:
    GameObject(id_t id);

private:
    id_t _id;
};
}  // namespace vge