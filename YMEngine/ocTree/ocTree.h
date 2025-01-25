#pragma once

#include "gameFrameWork/gameObject/gameObject.h"

namespace ym
{
    class Object;

    /**
     * �����s���E�{�b�N�X (Axis-Aligned Bounding Box) ��\���\����
     * min: AABB�̍ŏ��_�i�{�b�N�X�̍������j
     * max: AABB�̍ő�_�i�{�b�N�X�̉E���O�j
     */
    struct AABB {
        Vector3 min;  // �ŏ��_
        Vector3 max;  // �ő�_

        // �f�t�H���g�R���X�g���N�^
        AABB() : min(Vector3(0, 0, 0)), max(Vector3(0, 0, 0)) {}

        // �����t���R���X�g���N�^
        // @param min: AABB�̍ŏ��_
        // @param max: AABB�̍ő�_
        AABB(const Vector3 &min, const Vector3 &max) : min(min), max(max) {}

        /**
         * ����AABB�Ƃ̌���������s��
         * @param other: ��r�ΏۂƂȂ�AABB
         * @return ����AABB�ƌ������Ă���ꍇ��true�A�����łȂ��ꍇ��false
         */
        inline bool Intersects(const AABB &other) const {
            bool xOverlap = (min.x <= other.max.x && max.x >= other.min.x);
            bool yOverlap = (min.y <= other.max.y && max.y >= other.min.y);
            bool zOverlap = (min.z <= other.max.z && max.z >= other.min.z);

            return xOverlap && yOverlap && zOverlap;
        }
    };

    /**
     * �I�u�W�F�N�g��AABB���擾����
     * @param object: �ΏۃI�u�W�F�N�g
     * @return �I�u�W�F�N�g��AABB
     */
    AABB GetAABB(Object *object);

    /**
     * Octree�̃m�[�h�N���X
     * 1�̃m�[�h����Ԃ͈̔͂ƁA�����ɑ�����I�u�W�F�N�g��ێ�
     */
    class Node {
    public:
        AABB bounds;                      // �m�[�h�̋�Ԕ͈�
        std::vector<Object *> objects;    // �m�[�h�ɑ�����I�u�W�F�N�g
        std::array<std::unique_ptr<Node>, 8> children;  // �q�m�[�h

        /**
         * �m�[�h�̃R���X�g���N�^
         * @param bounds: ���̃m�[�h�̋�Ԕ͈͂�\��AABB
         */
        Node(const AABB &bounds) : bounds(bounds) {}
    };

    /**
     * Octree�N���X
     * 3������Ԃ𕪊����ĊǗ����A�����ȋ�Ԍ������\�ɂ���f�[�^�\��
     */
    class Octree {
    private:
        std::unique_ptr<Node> root;   // ���[�g�m�[�h
        int maxDepth;                 // �c���[�̍ő�[�x
        int maxObjectsPerNode;        // �m�[�h������̍ő�I�u�W�F�N�g��

    public:
        /**
         * �R���X�g���N�^
         * @param rootBounds: ���[�g�m�[�h�̋�Ԕ͈͂�\��AABB
         * @param maxDepth: �c���[�̍ő�[�x
         * @param maxObjects: �m�[�h�Ɋi�[�ł���ő�I�u�W�F�N�g��
         */
        Octree(const AABB &rootBounds, int maxDepth, int maxObjects)
            : root(std::make_unique<Node>(rootBounds)),
            maxDepth(maxDepth),
            maxObjectsPerNode(maxObjects) {}

        /**
         * �I�u�W�F�N�g��Octree�ɑ}������
         * @param object: �}������I�u�W�F�N�g
         */
        void Insert(Object *object) {
            Insert(root.get(), object, 0);
        }

        /**
         * �I�u�W�F�N�g��Octree����폜����
         * @param object: �폜����I�u�W�F�N�g
         */
        void Remove(Object *object) {
            Remove(root.get(), object);
        }

        /**
         * �w�肳�ꂽ�̈���̃I�u�W�F�N�g����������
         * @param area: �����Ώۂ̗̈��\��AABB
         * @param results: ���ʂ��i�[���邽�߂̃I�u�W�F�N�g�̃��X�g
         */
        void Query(const AABB &area, std::vector<Object *> &results) {
            Query(root.get(), area, results);
        }

    private:
        /**
         * �m�[�h��8�ɕ�������i�q�m�[�h���쐬�j
         * @param node: �����Ώۂ̃m�[�h
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
         * �m�[�h�ɃI�u�W�F�N�g��}������
         * @param node: �Ώۂ̃m�[�h
         * @param object: �}������I�u�W�F�N�g
         * @param depth: ���݂̐[��
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
         * �m�[�h����I�u�W�F�N�g���폜����
         * @param node: �Ώۂ̃m�[�h
         * @param object: �폜����I�u�W�F�N�g
         */
        void Remove(Node *node, Object *object) {
            if (!node->bounds.Intersects(GetAABB(object))) return;

            // ���݂̃m�[�h����I�u�W�F�N�g���폜
            auto it = std::find(node->objects.begin(), node->objects.end(), object);
            if (it != node->objects.end()) {
                node->objects.erase(it);
            }

            // �q�m�[�h�����݂���ꍇ�A����ɍċA�I�ɒT��
            if (node->children[0] != nullptr) {
                for (auto &child : node->children) {
                    Remove(child.get(), object);
                }
            }
        }

        /**
         * �m�[�h���̎w��̈�Ɋ܂܂��I�u�W�F�N�g����������
         * @param node: �Ώۂ̃m�[�h
         * @param area: �����Ώۂ̗̈�
         * @param results: ���ʂ��i�[���邽�߂̃I�u�W�F�N�g�̃��X�g
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
