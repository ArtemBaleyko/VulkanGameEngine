#include "Camera.h"

#include <cassert>
#include <limits>

namespace vge {
void Camera::setOrthographicProjection(float left, float right, float top, float bottom, float near, float far) {
    _projectionMatrix = glm::mat4{1.0f};
    _projectionMatrix[0][0] = 2.f / (right - left);
    _projectionMatrix[1][1] = 2.f / (bottom - top);
    _projectionMatrix[2][2] = 1.f / (far - near);
    _projectionMatrix[3][0] = -(right + left) / (right - left);
    _projectionMatrix[3][1] = -(bottom + top) / (bottom - top);
    _projectionMatrix[3][2] = -near / (far - near);
}

void Camera::setPerspectiveProjection(float fovy, float aspectRatio, float near, float far) {
    assert(glm::abs(aspectRatio - std::numeric_limits<float>::epsilon()) > 0.0f);
    const float tanHalfFovy = tan(fovy / 2.f);
    _projectionMatrix = glm::mat4{0.0f};
    _projectionMatrix[0][0] = 1.f / (aspectRatio * tanHalfFovy);
    _projectionMatrix[1][1] = 1.f / (tanHalfFovy);
    _projectionMatrix[2][2] = far / (far - near);
    _projectionMatrix[2][3] = 1.f;
    _projectionMatrix[3][2] = -(far * near) / (far - near);
}
}  // namespace vge