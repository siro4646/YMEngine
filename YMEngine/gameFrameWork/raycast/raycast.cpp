#include "raycast.h"

namespace ym
{
    bool Raycast(const Ray &ray, float maxDistance, RaycastHit &outHit, const std::vector<std::shared_ptr<Object>> &objects)
    {
        std::vector<Object *> rawObjects;
        for (const auto &obj : objects) {
            rawObjects.push_back(obj.get());
        }
        return Raycast(ray, maxDistance, outHit, rawObjects);
    }


    bool Raycast(const Ray &ray, float maxDistance, RaycastHit &outHit, const std::vector<Object *> &objects)
    {
        bool hit = false;
        float closestDistance = maxDistance;

        for (auto obj : objects)
        {
            auto collider = obj->GetComponent<Collider>().get();
            if (!collider) continue;

            float t = 0.0f;
            Vector3 hitPoint;

            if (collider->Raycast(ray, t, hitPoint)) // ← Collider に追加する関数
            {
                if (t < closestDistance)
                {
                    closestDistance = t;
                    outHit.point = hitPoint;
                    outHit.distance = t;
                    outHit.hitObject = obj;
                    // 衝突面の法線なども追加するなら、コライダーごとの関数で求める
                    hit = true;
                }
            }
        }

        return hit;
    }
}