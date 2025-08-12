#pragma once

#include <vector>
#include "SceneObject.h"

struct BoundingBox {
    float xMin;
    float zMin;
    float xMax;
    float zMax;

    // Check if the bounding box contains a SceneObject
    bool contains(const SceneObject* obj) const;

    // Check if this bounding box intersects another
    bool intersects(const BoundingBox& other) const;
};

class QuadtreeNode {
public:
    QuadtreeNode(const BoundingBox& bounds, int capacity);
    ~QuadtreeNode();

    bool insert(SceneObject* obj);
    void query(const BoundingBox& range, std::vector<SceneObject*>& found) const;

private:
    void subdivide();

    BoundingBox boundary;
    int nodeCapacity;
    bool divided;

    std::vector<SceneObject*> objects;

    QuadtreeNode* northeast = nullptr;
    QuadtreeNode* northwest = nullptr;
    QuadtreeNode* southeast = nullptr;
    QuadtreeNode* southwest = nullptr;
};
