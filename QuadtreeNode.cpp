#include "QuadtreeNode.h"
#include <glm/glm.hpp>
#include <algorithm>

// BoundingBox method implementations
bool BoundingBox::contains(const SceneObject* obj) const {
    float x = obj->getPosition().x;
    float z = obj->getPosition().z;
    return (x >= xMin && x <= xMax && z >= zMin && z <= zMax);
}

bool BoundingBox::intersects(const BoundingBox& other) const {
    return !(other.xMin > xMax || other.xMax < xMin || other.zMin > zMax || other.zMax < zMin);
}

// QuadtreeNode class methods
QuadtreeNode::QuadtreeNode(const BoundingBox& bounds, int capacity)
    : boundary(bounds), nodeCapacity(capacity), divided(false) {}

QuadtreeNode::~QuadtreeNode() {
    for (SceneObject* obj : objects)
        delete obj;

    delete northeast;
    delete northwest;
    delete southeast;
    delete southwest;
}

bool QuadtreeNode::insert(SceneObject* obj) {
    if (!boundary.contains(obj))
        return false;

    if (objects.size() < nodeCapacity) {
        objects.push_back(obj);
        return true;
    }

    if (!divided)
        subdivide();

    return (northeast->insert(obj) || northwest->insert(obj) ||
        southeast->insert(obj) || southwest->insert(obj));
}

void QuadtreeNode::subdivide() {
    float xMid = (boundary.xMin + boundary.xMax) / 2.0f;
    float zMid = (boundary.zMin + boundary.zMax) / 2.0f;

    northeast = new QuadtreeNode({ xMid, boundary.zMin, boundary.xMax, zMid }, nodeCapacity);
    northwest = new QuadtreeNode({ boundary.xMin, boundary.zMin, xMid, zMid }, nodeCapacity);
    southeast = new QuadtreeNode({ xMid, zMid, boundary.xMax, boundary.zMax }, nodeCapacity);
    southwest = new QuadtreeNode({ boundary.xMin, zMid, xMid, boundary.zMax }, nodeCapacity);

    divided = true;
}

void QuadtreeNode::query(const BoundingBox& range, std::vector<SceneObject*>& found) const {
    if (!boundary.intersects(range))
        return;

    for (SceneObject* obj : objects) {
        if (range.contains(obj))
            found.push_back(obj);
    }

    if (divided) {
        northeast->query(range, found);
        northwest->query(range, found);
        southeast->query(range, found);
        southwest->query(range, found);
    }
}
