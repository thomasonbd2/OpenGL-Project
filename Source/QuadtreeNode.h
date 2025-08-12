#pragma once

#include <vector>
#include "SceneObject.h"

struct BoundingBox {
    float xMin;
    float zMin;
    float xMax;
    float zMax;

    bool contains(const SceneObject* obj) const;
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

    QuadtreeNode* northeast;
    QuadtreeNode* northwest;
    QuadtreeNode* southeast;
    QuadtreeNode* southwest;
};
