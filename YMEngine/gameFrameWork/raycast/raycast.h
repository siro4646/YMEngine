#pragma once
#include "gameObject/gameObject.h"
#include "gameFrameWork/collider/collider.h"
namespace ym
{
	class Collider; // 前方宣言
	class Object; // 前方宣言
	class Vector3; // 前方宣言
	class BoxCollider; // 前方宣言
	class SphereCollider; // 前方宣言

    struct Ray {
        Vector3 origin;
        Vector3 direction;

        Ray(const Vector3 &o, const Vector3 &d) : origin(o), direction(d.Normalize()) {}
    };

    struct RaycastHit {
        Vector3 point;         // 衝突地点
        Vector3 normal;        // 衝突面の法線
        float distance;        // 原点からの距離
        Object *hitObject = nullptr;  // 衝突したオブジェクト
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