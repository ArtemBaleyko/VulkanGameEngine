#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace vge {
class Camera {
public:
    void setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
    void setPerspectiveProjection(float fovy, float aspectRatio, float near, float far);

    void setViewDirection(const glm::vec3& position,
                          const glm::vec3& direction,
                          const glm::vec3& up = {0.0f, -1.0f, 0.0f});

    void setViewTarget(const glm::vec3& position,
                       const glm::vec3& target,
                       const glm::vec3& up = {0.0f, -1.0f, 0.0f});

    void setViewYXZ(const glm::vec3& position, const glm::vec3& rotation);

    inline const glm::mat4& getProjectionMatrix() const { return _projectionMatrix; }
    inline const glm::mat4& getViewMatrix() const { return _viewMatrix; }
    inline const glm::mat4& getInverseViewMatrix() const { return _inverseViewMatrix; }
    inline const glm::mat4  getProjectionViewMatrix() const { return _projectionMatrix * _viewMatrix; }

private:
    glm::mat4 _projectionMatrix{1.0f};
    glm::mat4 _viewMatrix{1.0f};
    glm::mat4 _inverseViewMatrix{1.0f};
};
}  // namespace vge