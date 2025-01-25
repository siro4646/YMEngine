#include "rigidBody.h"
#include "gameFrameWork/gameObject/gameObject.h"
#include "application/application.h"
#include "ocTree/ocTree.h"
#include "gameFrameWork/collider/collider.h"

#include <execution> // 並列実行ポリシーを利用する場合
#include <thread>

namespace ym {

    // ================================
    // ヘルパー関数群
    // ================================

	//=================================
	//プロトタイプ宣言
	//=================================
	Vector3 CalculateContactPoint(const Collider &a, const Collider &b);
	Vector3 CalculateCollisionNormal(const Collider &a, const Collider &b);
	Vector3 CalculateOBBCollisionNormal(const BoxCollider &a, const BoxCollider &b);
	Vector3 CalculateCollisionNormal(const SphereCollider &sphere, const BoxCollider &box);
	Vector3 CalculateCollisionNormal(const SphereCollider &sphereA, const SphereCollider &sphereB);
	float CalculateAxisOverlap(const BoxCollider &a, const BoxCollider &b, const Vector3 &axis);


    // 衝突点を計算（仮実装）
    Vector3 CalculateContactPoint(const Collider &a, const Collider &b) {
        return (a.GetCenter() + b.GetCenter()) * 0.5f;
    }

    // 衝突法線を計算（Colliderの種類ごとに分岐）
     Vector3 CalculateCollisionNormal(const Collider &a, const Collider &b) {
        if (a.GetColliderType() == ColliderType::Box && b.GetColliderType() == ColliderType::Box) {
            return CalculateOBBCollisionNormal(static_cast<const BoxCollider &>(a), static_cast<const BoxCollider &>(b));
        }
        if (a.GetColliderType() == ColliderType::Sphere && b.GetColliderType() == ColliderType::Box) {
            return CalculateCollisionNormal(static_cast<const SphereCollider &>(a), static_cast<const BoxCollider &>(b));
        }
        if (a.GetColliderType() == ColliderType::Box && b.GetColliderType() == ColliderType::Sphere) {
            return CalculateCollisionNormal(static_cast<const SphereCollider &>(b), static_cast<const BoxCollider &>(a));
        }
        if (a.GetColliderType() == ColliderType::Sphere && b.GetColliderType() == ColliderType::Sphere) {
            return CalculateCollisionNormal(static_cast<const SphereCollider &>(a), static_cast<const SphereCollider &>(b));
        }
        return Vector3::zero; // デフォルト（未対応の場合）
    }

     float CalculateAxisOverlap(const BoxCollider &a, const BoxCollider &b, const Vector3 &axis) {
         auto axesA = a.GetLocalAxes();
         // Dot計算を変数に一度だけ格納する方法
         float dotA0 = axesA[0].Dot(axis);
         float dotA1 = axesA[1].Dot(axis);
         float dotA2 = axesA[2].Dot(axis);

		 auto aScale = a.GetScale();
		 auto bScale = b.GetScale();

         float aProjection =
             aScale.x * std::abs(dotA0) +
             aScale.y * std::abs(dotA1) +
             aScale.z * std::abs(dotA2);

         auto axesB = b.GetLocalAxes();
         float dotB0 = axesB[0].Dot(axis);
         float dotB1 = axesB[1].Dot(axis);
         float dotB2 = axesB[2].Dot(axis);

         float bProjection =
             bScale.x * std::abs(dotB0) +
             bScale.y * std::abs(dotB1) +
             bScale.z * std::abs(dotB2);

         float distance = std::abs((b.GetCenter() - a.GetCenter()).Dot(axis));
         return (aProjection + bProjection) - distance;
     }

    // OBB間の衝突法線を計算
     Vector3 CalculateOBBCollisionNormal(const BoxCollider &a, const BoxCollider &b) {
         auto axesA = a.GetLocalAxes();
         auto axesB = b.GetLocalAxes();

         Vector3 normal = Vector3::zero;
         float minOverlap = std::numeric_limits<float>::max();

         // --- 以下のfor文で全ての軸について重なりを求める ---
         for (int i = 0; i < 3; ++i) {
             float overlap = CalculateAxisOverlap(a, b, axesA[i]);
             // もしoverlapが0以下なら衝突なしとみなし、すぐに処理を打ち切る手もある
             if (overlap <= 0.0f) {
                 return Vector3::zero; // 衝突なし（法線はゼロベクトル）
             }
             if (overlap < minOverlap) {
                 minOverlap = overlap;
                 normal = axesA[i];
             }
         }

         for (int i = 0; i < 3; ++i) {
             float overlap = CalculateAxisOverlap(a, b, axesB[i]);
             if (overlap <= 0.0f) {
                 return Vector3::zero;
             }
             if (overlap < minOverlap) {
                 minOverlap = overlap;
                 normal = axesB[i];
             }
         }

         return normal.Normalize();
     }


    // SphereとBox間の衝突法線を計算
    Vector3 CalculateCollisionNormal(const SphereCollider &sphere, const BoxCollider &box) {
        Vector3 normal = sphere.GetCenter() - box.GetClosestPoint(sphere.GetCenter());
        if (normal.LengthSquared() > 0) {
            return normal.Normalize();
        }
        return Vector3(0, 1, 0); // デフォルト法線
    }

    // Sphere間の衝突法線を計算
    Vector3 CalculateCollisionNormal(const SphereCollider &sphereA, const SphereCollider &sphereB) {
        Vector3 normal = sphereB.GetCenter() - sphereA.GetCenter();
        if (normal.LengthSquared() > 0) {
            return normal.Normalize();
        }
        return Vector3(0, 1, 0); // デフォルト法線
    }

    // ================================
    // Rigidbody クラス
    // ================================

    void Rigidbody::UpdateForceAndAcceleration() {
        if (velocity.LengthSquared() < 0.0001f && isGround) {
            return; // 今回のフレームは何もしない
        }
        if (useGravity && !isGround) {
            AddForce(Vector3(0, -GRAVITY, 0), ForceMode::Acceleration);
        }
    }

    void Rigidbody::DetectAndResolveCollisions() {
        result.clear();
        previousCollisions = currentCollisions;
        currentCollisions.clear();
        previousTriggers = currentTriggers;
        currentTriggers.clear();

        //auto collider = object->GetComponent<Collider>();
        if (!collider) return;

        // オクツリーで候補を取得
        AABB ownAABB = GetAABB(object);
        sceneOctree->Query(ownAABB, result);

        AsyncHitTest();
        //HitTest();


        // 衝突終了処理
        for (auto obj : previousCollisions) {
            if (currentCollisions.find(obj) == currentCollisions.end()) {
                for (auto &component : object->components) {
                    Collision collision;
                    collision.object = obj;
                    collision.collider = obj->GetComponent<Collider>().get();
                    component->OnCollisionExit(collision);
                }
            }
        }

        // トリガー終了処理
        for (auto obj : previousTriggers) {
            if (currentTriggers.find(obj) == currentTriggers.end()) {
                for (auto &component : object->components) {
                    Collision collision;
                    collision.object = obj;
                    collision.collider = obj->GetComponent<Collider>().get();
                    component->OnTriggerExit(collision);
                }
            }
        }
    }

    void Rigidbody::UpdatePosition() {
        object->localTransform.Position += velocity * Application::Instance()->GetDeltaTime();
    }

    void Rigidbody::UpdateVelocity() {
        velocity += force;
        auto preVelocity = velocity;

        if (velocity.Length() > 0) {
            velocity *= (1.0f - drag * Application::Instance()->GetDeltaTime());
        }

        if (isGround) {
            float frictionCoefficient = 0.0f;
            Vector3 friction = -velocity.Normalize() * frictionCoefficient * Application::Instance()->GetDeltaTime();
            velocity += friction;

            if (velocity.Length() < 0.01f) {
                velocity = Vector3::zero;
            }
        }
    }

    //bool Rigidbody::IsOnGround() {
    //    isGround = false;

    //    // 現在のオブジェクトのコライダーを取得
    //    //auto collider = object->GetComponent<Collider>();
    //    if (!collider) return false; // コライダーがない場合は即座に終了

    //    // 衝突候補リストをチェック
    //    for (auto obj : result) {
    //        if (obj == object) continue; // 自分自身はスキップ

    //        auto otherCollider = obj->GetComponent<Collider>();
    //        if (!otherCollider) continue; // 他のオブジェクトにコライダーがなければスキップ

    //        // 衝突しているかを確認
    //        if (collider->IsColliding(*otherCollider)) {
    //            // 衝突法線を計算
    //            Vector3 normal = CalculateCollisionNormal(*collider, *otherCollider);

    //            // 法線が上向きの場合（Y軸が正の値）を接地として判断
    //            if (normal.y > 0.5f) {
    //                isGround = true;
    //                return true;
    //            }
    //        }
    //    }
    //    return false;
    //}

    void Rigidbody::HitTest()
    {
        for (auto obj : result) {
            if (obj == object) continue;
            auto otherCollider = obj->GetComponent<Collider>();
            if (!otherCollider) continue;

            // 衝突判定
            if (collider->IsColliding(*otherCollider)) {
                if (collider->isTrigger || otherCollider->isTrigger) {
                    // トリガー処理
                    currentTriggers.insert(obj);
                    Collision triggerCollision = Collision::CreateCollision(otherCollider.get(), obj, Vector3::zero, Vector3::zero);

                    if (previousTriggers.find(obj) == previousTriggers.end()) {
                        for (auto &component : object->components) {
                            component->OnTriggerEnter(triggerCollision);
                        }
                    }
                    else {
                        for (auto &component : object->components) {
                            component->OnTriggerStay(triggerCollision);
                        }
                    }
                    continue; // 物理演算をスキップ
                }

                // 通常の物理衝突処理
                currentCollisions.insert(obj);
                Vector3 contactPoint = CalculateContactPoint(*collider, *otherCollider);
                Vector3 normal = CalculateCollisionNormal(*collider, *otherCollider);

                Collision collision = Collision::CreateCollision(otherCollider.get(), obj, contactPoint, normal);

                if (previousCollisions.find(obj) == previousCollisions.end()) {
                    for (auto &component : object->components) {
                        component->OnCollisionEnter(collision);
                    }
                }
                else {
                    for (auto &component : object->components) {
                        component->OnCollisionStay(collision);
                    }
                }

                // 反発係数を使用した速度計算
                float restitution = 0.4f;
                velocity = velocity - (1 + restitution) * velocity.Dot(normal) * normal;
            }
        }
    }

    void Rigidbody::AsyncHitTest() {
        thread_local std::vector<Collision> localCollisions; // スレッドローカルの一時データ
        thread_local std::vector<Collision> localTriggers;

        // ★ここで事前にクリア＆リザーブしておく
        localCollisions.clear();
        localTriggers.clear();
        // 例えば result.size() くらいは衝突判定する可能性があるので確保しておく
        localCollisions.reserve(result.size());
        localTriggers.reserve(result.size());

		isGround = false;

        std::for_each(std::execution::par, result.begin(), result.end(), [&](auto &gameObject) {
            if (gameObject == object) return;

            auto otherCollider = gameObject->GetComponent<Collider>();
            if (!otherCollider) return;

            // 衝突判定
            if (collider->IsColliding(*otherCollider)) {
                Vector3 contactPoint = CalculateContactPoint(*collider, *otherCollider);
                Vector3 normal = CalculateCollisionNormal(*collider, *otherCollider);
                // 法線が上向きの場合（Y軸が正の値）を接地として判断
                if (normal.y > 0.5f) {
                    isGround = true;
                }

                if (collider->isTrigger || otherCollider->isTrigger) {
                    // トリガー処理
                    Collision triggerCollision = Collision::CreateCollision(otherCollider.get(), gameObject, contactPoint, normal);
                    localTriggers.push_back(triggerCollision);
                }
                else {
                    // 通常衝突処理
                    Collision collision = Collision::CreateCollision(otherCollider.get(), gameObject, contactPoint, normal);
                    localCollisions.push_back(collision);

                    // 反発係数を使用した速度計算
                    float restitution = 0.4f;
                    velocity = velocity - (1 + restitution) * velocity.Dot(normal) * normal;
                }
            }
            });

        // 結果をマージ（競合回避）
        {
            std::lock_guard<std::mutex> lock(mutex);
            for (auto &collision : localCollisions) {
                currentCollisions.insert(collision.object);
                for (auto &component : object->components) {
                    if (previousCollisions.find(collision.object) == previousCollisions.end()) {
                        component->OnCollisionEnter(collision);
                    }
                    else {
                        component->OnCollisionStay(collision);
                    }
                }
            }

            for (auto &trigger : localTriggers) {
                currentTriggers.insert(trigger.object);
                for (auto &component : object->components) {
                    if (previousTriggers.find(trigger.object) == previousTriggers.end()) {
                        component->OnTriggerEnter(trigger);
                    }
                    else {
                        component->OnTriggerStay(trigger);
                    }
                }
            }
        }
    }



} // namespace ym
