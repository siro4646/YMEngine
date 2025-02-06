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
		hitObjects.clear();

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
            float frictionCoefficient = 0.3f;
            Vector3 friction = -velocity.Normalize() * frictionCoefficient * Application::Instance()->GetDeltaTime();
            velocity += friction;

            if (velocity.Length() < 0.01f) {
                velocity = Vector3::zero;
            }
        }
    }

    void Rigidbody::ResolveCollision(Object *hitTarget, Vector3 normal)
    {
        //
        bool hasRigidbody = false;

		Rigidbody *hitRb = hitTarget->GetComponent<Rigidbody>().get();
        if (hitRb) {
            if (hitRb->CheckHitObject(object))
            {
                return;
            }
			hasRigidbody = true;
        }
        if (hasRigidbody) 
        {


        Vector3 scaleA = this->collider->GetScale();
        Vector3 scaleB = hitRb->collider->GetScale();

        Vector3 radSum = (scaleA + scaleB) * 0.5f;
		Vector3 distance = hitTarget->worldTransform.Position - object->worldTransform.Position;

		Vector3 penetration = radSum -Vector3(abs(distance.x), abs(distance.y), abs(distance.z));

        // 最も深いめり込み軸を特定
        Vector3 penetrationAxis = Vector3::zero;
        float minPenetration = std::numeric_limits<float>::max();

        if (penetration.x < minPenetration) {
            minPenetration = penetration.x;
            penetrationAxis = Vector3(1, 0, 0);
        }
        if (penetration.y < minPenetration) {
            minPenetration = penetration.y;
            penetrationAxis = Vector3(0, 1, 0);
        }
        if (penetration.z < minPenetration) {
            minPenetration = penetration.z;
            penetrationAxis = Vector3(0, 0, 1);
        }
        // 位置補正の適用（過補正を防ぐ）

        float correctionFactor = 1.05f; // 100% だと振動するため微調整
        float epsilon = 0.001f; // 小さな隙間を開ける
        Vector3 correction = normal * (minPenetration * correctionFactor + epsilon);



            // 質量比を考慮（軽い方がより押し戻される）
            float totalMass = mass + hitRb->mass;
            float massRatioA = (totalMass > 0) ? (hitRb->mass / totalMass) : 0.5f;
            float massRatioB = 1.0f - massRatioA;

            object->localTransform.Position += correction * massRatioA;
            hitTarget->localTransform.Position -= correction * massRatioB;

            // 反発係数の決定（両者の平均を取る）
            float restitution = std::min(this->restitution, hitRb->restitution);

            // 速度の分解（法線方向と接線方向）
            Vector3 relativeVelocity = velocity - hitRb->velocity;
            float normalVelocity = relativeVelocity.Dot(normal);

            float inverseMassA = (mass > 0) ? (1 / mass) : 0;
            float inverseMassB = (hitRb->mass > 0) ? (1 / hitRb->mass) : 0;

            float impulse = -(1 + restitution) * normalVelocity;
            impulse /= (inverseMassA + inverseMassB);

            if (normalVelocity < 0) { // 衝突している場合のみ適用
                velocity += impulse * normal * inverseMassA;
                hitRb->velocity -= impulse * normal * inverseMassB;
            }

			RegisterHitObject(hitTarget);
			hitRb->RegisterHitObject(object);
		}
		else {
            velocity = velocity - (1 + restitution) * velocity.Dot(normal) * normal;
        }
    }

    void Rigidbody::ResolveCollision(Object *hitTarget, Vector3 normal, Vector3 predictedPosition)
    {
        Rigidbody *hitRb = hitTarget->GetComponent<Rigidbody>().get();
        if (!hitRb) return; // Rigidbody がない場合は何もしない
        if (hitRb->CheckHitObject(object)) return; // すでに処理済みならスキップ

        // スケールの取得
        Vector3 scaleA = this->collider->GetScale();
        Vector3 scaleB = hitRb->collider->GetScale();
        Vector3 radSum = (scaleA + scaleB) * 0.5f;
        Vector3 distance = hitTarget->worldTransform.Position - predictedPosition;
        Vector3 penetration = radSum - Vector3(abs(distance.x), abs(distance.y), abs(distance.z));

        // **最大の penetrationDepth を取得**
        float penetrationDepth = std::max({ penetration.x, penetration.y, penetration.z });
        if (penetrationDepth <= 0.0f) return; // 衝突していない場合はスキップ

        // **位置補正の適用**
        float correctionFactor = 1.05f; // 過補正を防ぐ
        float epsilon = 0.001f; // 小さな隙間を開ける
        Vector3 correction = normal * (penetrationDepth * correctionFactor + epsilon);

        // **質量比を考慮**
        float totalMass = mass + hitRb->mass;
        float massRatioA = (totalMass > 0) ? (hitRb->mass / totalMass) : 0.5f;
        float massRatioB = 1.0f - massRatioA;

        // **未来位置を適用**
        Vector3 correctedPosition = predictedPosition + correction * massRatioA;

        // **オブジェクトの位置を更新**
        object->localTransform.Position = correctedPosition;

        // **衝突リストに登録**
        RegisterHitObject(hitTarget);
        hitRb->RegisterHitObject(object);
    }



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
               // float restitution = 0.4f;
                velocity = velocity - (1 + restitution) * velocity.Dot(normal) * normal;
            }
        }
    }

    void Rigidbody::AsyncHitTest()
    {
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

                    // obj->worldTransform.Position -= velocity;

                    // ResolveCollision(gameObject, normal);

                     // 反発係数を使用した速度計算
                    // float restitution = 0.4f;
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
                for (auto &component : trigger.object->components)
                {
                    if (previousTriggers.find(trigger.object) == previousTriggers.end())
                    {
						Collision triggerCollision = Collision::CreateCollision(collider, object, trigger.contactPoint, trigger.normal);
                        component->OnTriggerEnter(triggerCollision);
                    }
                    else
                    {
						Collision triggerCollision = Collision::CreateCollision(collider, object, trigger.contactPoint, trigger.normal);
						component->OnTriggerStay(triggerCollision);
                    }
                }


            }
            //OntriggerExit

			for (auto obj : previousTriggers)
            {
                if (currentTriggers.find(obj) == currentTriggers.end())
                {
                    for (auto &component : object->components)
                    {
						Collision triggerCollision = Collision::CreateCollision(obj->GetComponent<Collider>().get(), obj, Vector3::zero, Vector3::zero);
						component->OnTriggerExit(triggerCollision);
					}
                    //トリガー側も
                    for (auto &component : obj->components)
                    {
						Collision triggerCollision = Collision::CreateCollision(collider, object, Vector3::zero, Vector3::zero);
						component->OnTriggerExit(triggerCollision);
                    }
				}
			}


        }
    }
    



} // namespace ym
