#pragma once
#include "gameObject/gameObject.h"
#include "gameFrameWork/collider/collider.h"
namespace ym
{
	class Collider; // �O���錾
	class Object; // �O���錾
	class Vector3; // �O���錾
	class BoxCollider; // �O���錾
	class SphereCollider; // �O���錾

    struct Ray {
        Vector3 origin;
        Vector3 direction;

        Ray(const Vector3 &o, const Vector3 &d) : origin(o), direction(d.Normalize()) {}
    };

    struct RaycastHit {
        Vector3 point;         // �Փ˒n�_
        Vector3 normal;        // �Փ˖ʂ̖@��
        float distance;        // ���_����̋���
        Object *hitObject = nullptr;  // �Փ˂����I�u�W�F�N�g
    };

    struct RayHitInfo
    {
        bool hit = false;
        Vector3 hitPoint = Vector3::zero;
        Vector3 rayOrigin = Vector3::zero;
        Vector3 rayDir = Vector3::zero;
        std::string hitObjectName;
        float distance = 0.0f;
    };

	bool Raycast(const Ray &ray, float maxDistance, RaycastHit &outHit, const std::vector<Object *> &objects);

    bool Raycast(const Ray &ray, float maxDistance, RaycastHit &outHit, const std::vector<std::shared_ptr<Object>> &objects);


}