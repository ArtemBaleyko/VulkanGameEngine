#include "GameObject.h"

namespace vge {
GameObject GameObject::createGameObject() {
    static id_t currentId = 0;
    return GameObject(currentId++);
}

GameObject::GameObject(id_t id)
    : color{}
    , model{}
    , transform2D{}
    , _id(id) {}

}  // namespace vge
