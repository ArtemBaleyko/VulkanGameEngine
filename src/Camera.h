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

    inline const glm::mat4& getProjectionMatrix() const { return _projectionMatrix; }

private:
    glm::mat4 _projectionMatrix{1.0f};
};
}  // namespace vge