#pragma once
#include <glm/glm.hpp>
#include <string>

class SceneObject {
public:
    std::string name;
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;
    int id;

    SceneObject(const std::string& name, glm::vec3 position, int id = 0)
        : name(name), position(position), scale(1.0f), rotation(0.0f), id(id) {}

    glm::vec3 getPosition() const { return position; }
};
