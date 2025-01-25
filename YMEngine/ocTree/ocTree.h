#pragma once

#include "gameFrameWork/gameObject/gameObject.h"

namespace ym
{
    class Object;

    /**
     * 軸平行境界ボックス (Axis-Aligned Bounding Box) を表す構造体
     * min: AABBの最小点（ボックスの左下奥）
     * max: AABBの最大点（ボックスの右上手前）
     */
    struct AABB {
        Vector3 min;  // 最小点
        Vector3 max;  // 最大点

        // デフォルトコンストラクタ
        AABB() : min(Vector3(0, 0, 0)), max(Vector3(0, 0, 0)) {}

        // 引数付きコンストラクタ
        // @param min: AABBの最小点
        // @param max: AABBの最大点
        AABB(const Vector3 &min, const Vector3 &max) : min(min), max(max) {}

        /**
         * 他のAABBとの交差判定を行う
         * @param other: 比較対象となるAABB
         * @return 他のAABBと交差している場合はtrue、そうでない場合はfalse
         */
        inline bool Intersects(const AABB &other) const {
            bool xOverlap = (min.x <= other.max.x && max.x >= other.min.x);
            bool yOverlap = (min.y <= other.max.y && max.y >= other.min.y);
            bool zOverlap = (min.z <= other.max.z && max.z >= other.min.z);

            return xOverlap && yOverlap && zOverlap;
        }
    };

    /**
     * オブジェクトのAABBを取得する
     * @param object: 対象オブジェクト
     * @return オブジェクトのAABB
     */
    AABB GetAABB(Object *object);

    /**
     * Octreeのノードクラス
     * 1つのノードが空間の範囲と、そこに属するオブジェクトを保持
     */
    class Node {
    public:
        AABB bounds;                      // ノードの空間範囲
        std::vector<Object *> objects;    // ノードに属するオブジェクト
        std::array<std::unique_ptr<Node>, 8> children;  // 子ノード

        /**
         * ノードのコンストラクタ
         * @param bounds: このノードの空間範囲を表すAABB
         */
        Node(const AABB &bounds) : bounds(bounds) {}
    };

    /**
     * Octreeクラス
     * 3次元空間を分割して管理し、高速な空間検索を可能にするデータ構造
     */
    class Octree {
    private:
        std::unique_ptr<Node> root;   // ルートノード
        int maxDepth;                 // ツリーの最大深度
        int maxObjectsPerNode;        // ノードあたりの最大オブジェクト数

    public:
        /**
         * コンストラクタ
         * @param rootBounds: ルートノードの空間範囲を表すAABB
         * @param maxDepth: ツリーの最大深度
         * @param maxObjects: ノードに格納できる最大オブジェクト数
         */
        Octree(const AABB &rootBounds, int maxDepth, int maxObjects)
            : root(std::make_unique<Node>(rootBounds)),
            maxDepth(maxDepth),
            maxObjectsPerNode(maxObjects) {}

        /**
         * オブジェクトをOctreeに挿入する
         * @param object: 挿入するオブジェクト
         */
        void Insert(Object *object) {
            Insert(root.get(), object, 0);
        }

        /**
         * オブジェクトをOctreeから削除する
         * @param object: 削除するオブジェクト
         */
        void Remove(Object *object) {
            Remove(root.get(), object);
        }

        /**
         * 指定された領域内のオブジェクトを検索する
         * @param area: 検索対象の領域を表すAABB
         * @param results: 結果を格納するためのオブジェクトのリスト
         */
        void Query(const AABB &area, std::vector<Object *> &results) {
            Query(root.get(), area, results);
        }

    private:
        /**
         * ノードを8つに分割する（子ノードを作成）
         * @param node: 分割対象のノード
         */
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

        /**
         * ノードにオブジェクトを挿入する
         * @param node: 対象のノード
         * @param object: 挿入するオブジェクト
         * @param depth: 現在の深さ
         */
        void Insert(Node *node, Object *object, int depth) {
            if (!node->bounds.Intersects(GetAABB(object))) return;

            if (depth < maxDepth && node->objects.size() >= maxObjectsPerNode) {
                if (node->children[0] == nullptr) Subdivide(node);
                for (auto &child : node->children) {
                    Insert(child.get(), object, depth + 1);
                }
            }
            else {
                node->objects.push_back(object);
            }
        }

        /**
         * ノードからオブジェクトを削除する
         * @param node: 対象のノード
         * @param object: 削除するオブジェクト
         */
        void Remove(Node *node, Object *object) {
            if (!node->bounds.Intersects(GetAABB(object))) return;

            // 現在のノードからオブジェクトを削除
            auto it = std::find(node->objects.begin(), node->objects.end(), object);
            if (it != node->objects.end()) {
                node->objects.erase(it);
            }

            // 子ノードが存在する場合、さらに再帰的に探索
            if (node->children[0] != nullptr) {
                for (auto &child : node->children) {
                    Remove(child.get(), object);
                }
            }
        }

        /**
         * ノード内の指定領域に含まれるオブジェクトを検索する
         * @param node: 対象のノード
         * @param area: 検索対象の領域
         * @param results: 結果を格納するためのオブジェクトのリスト
         */
        void Query(Node *node, const AABB &area, std::vector<Object *> &results) {
            if (!node->bounds.Intersects(area)) return;

            for (auto *object : node->objects) {
                if (GetAABB(object).Intersects(area)) {
                    results.push_back(object);
                }
            }

            if (node->children[0] != nullptr) {
                for (auto &child : node->children) {
                    Query(child.get(), area, results);
                }
            }
        }
    };
}
