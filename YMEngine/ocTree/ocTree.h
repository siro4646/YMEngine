#pragma once

#include "gameFrameWork/gameObject/gameObject.h"

namespace ym
{
    class Object;

    struct AABB {
        Vector3 min;
        Vector3 max;

        AABB() : min(Vector3(0, 0, 0)), max(Vector3(0, 0, 0)) {}
        AABB(const Vector3 &min, const Vector3 &max) : min(min), max(max) {}

        inline bool Intersects(const AABB &other) const {
            return (min.x <= other.max.x && max.x >= other.min.x) &&
                (min.y <= other.max.y && max.y >= other.min.y) &&
                (min.z <= other.max.z && max.z >= other.min.z);
        }
    };

    AABB GetAABB(Object *object);

    class Node {
    public:
        AABB bounds;
        std::vector<Object *> objects;
        std::array<std::unique_ptr<Node>, 8> children;

        Node(const AABB &bounds) : bounds(bounds) {}
    };

    class Octree {
    private:
        std::unique_ptr<Node> root;
        int maxDepth;
        int maxObjectsPerNode;

    public:
        Octree(const AABB &rootBounds, int maxDepth, int maxObjects)
            : root(std::make_unique<Node>(rootBounds)),
            maxDepth(maxDepth),
            maxObjectsPerNode(maxObjects) {}

        void Insert(Object *object) {
            if (!object) return;
            Insert(root.get(), object, 0);
        }

        void Remove(Object *object) {
            if (!object || object->type == Object::Type::Delete) return;
            Remove(root.get(), object);
        }

        void Query(const AABB &area, std::vector<Object *> &results) {
            Query(root.get(), area, results);
        }

    private:
        void Subdivide(Node *node) {
            Vector3 min = node->bounds.min;
            Vector3 max = node->bounds.max;
            Vector3 mid = (min + max) * 0.5f;

            node->children[0] = std::make_unique<Node>(AABB(min, mid));
            node->children[1] = std::make_unique<Node>(AABB(Vector3(mid.x, min.y, min.z), Vector3(max.x, mid.y, mid.z)));
            node->children[2] = std::make_unique<Node>(AABB(Vector3(min.x, mid.y, min.z), Vector3(mid.x, max.y, mid.z)));
            node->children[3] = std::make_unique<Node>(AABB(Vector3(mid.x, mid.y, min.z), max));
            node->children[4] = std::make_unique<Node>(AABB(Vector3(min.x, min.y, mid.z), Vector3(mid.x, mid.y, max.z)));
            node->children[5] = std::make_unique<Node>(AABB(Vector3(mid.x, min.y, mid.z), Vector3(max.x, mid.y, max.z)));
            node->children[6] = std::make_unique<Node>(AABB(Vector3(min.x, mid.y, mid.z), Vector3(mid.x, max.y, max.z)));
            node->children[7] = std::make_unique<Node>(AABB(mid, max));
        }

        void Insert(Node *node, Object *object, int depth) {
            AABB aabb = GetAABB(object);
            if (!node->bounds.Intersects(aabb)) return;

            if (depth < maxDepth && node->objects.size() >= maxObjectsPerNode) {
                if (!node->children[0]) Subdivide(node);

                for (auto &child : node->children) {
                    if (child->bounds.Intersects(aabb)) {
                        Insert(child.get(), object, depth + 1);
                        return;
                    }
                }
            }

            node->objects.push_back(object);
        }

        void Remove(Node *node, Object *object) {
            AABB aabb = GetAABB(object);
            if (!node->bounds.Intersects(aabb)) return;

            auto it = std::remove(node->objects.begin(), node->objects.end(), object);
            if (it != node->objects.end()) {
                node->objects.erase(it, node->objects.end());
            }

            if (node->children[0]) {
                for (auto &child : node->children) {
                    Remove(child.get(), object);
                }
            }
        }

        void Query(Node *node, const AABB &area, std::vector<Object *> &results) {
            if (!node->bounds.Intersects(area)) return;

            for (auto *object : node->objects) {
                if (object && GetAABB(object).Intersects(area)) {
                    results.push_back(object);
                }
            }

            if (node->children[0]) {
                for (auto &child : node->children) {
                    Query(child.get(), area, results);
                }
            }
        }
    };
}
