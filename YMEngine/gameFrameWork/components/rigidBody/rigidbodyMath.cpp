#include "rigidBody.h"
#include "gameFrameWork/gameObject/gameObject.h"
#include "application/application.h"
#include "ocTree/ocTree.h"
#include "gameFrameWork/collider/collider.h"

#include <execution> // ������s�|���V�[�𗘗p����ꍇ
#include <thread>

namespace ym {

    // ================================
    // �w���p�[�֐��Q
    // ================================

	//=================================
	//�v���g�^�C�v�錾
	//=================================
	Vector3 CalculateContactPoint(const Collider &a, const Collider &b);
	Vector3 CalculateCollisionNormal(const Collider &a, const Collider &b);
	Vector3 CalculateOBBCollisionNormal(const BoxCollider &a, const BoxCollider &b);
	Vector3 CalculateCollisionNormal(const SphereCollider &sphere, const BoxCollider &box);
	Vector3 CalculateCollisionNormal(const SphereCollider &sphereA, const SphereCollider &sphereB);
	float CalculateAxisOverlap(const BoxCollider &a, const BoxCollider &b, const Vector3 &axis);


    // �Փ˓_���v�Z�i�������j
    Vector3 CalculateContactPoint(const Collider &a, const Collider &b) {
        return (a.GetCenter() + b.GetCenter()) * 0.5f;
    }

    // �Փ˖@�����v�Z�iCollider�̎�ނ��Ƃɕ���j
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
        return Vector3::zero; // �f�t�H���g�i���Ή��̏ꍇ�j
    }

     float CalculateAxisOverlap(const BoxCollider &a, const BoxCollider &b, const Vector3 &axis) {
         auto axesA = a.GetLocalAxes();
         // Dot�v�Z��ϐ��Ɉ�x�����i�[������@
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

    // OBB�Ԃ̏Փ˖@�����v�Z
     Vector3 CalculateOBBCollisionNormal(const BoxCollider &a, const BoxCollider &b) {
         auto axesA = a.GetLocalAxes();
         auto axesB = b.GetLocalAxes();

         Vector3 normal = Vector3::zero;
         float minOverlap = std::numeric_limits<float>::max();

         // --- �ȉ���for���őS�Ă̎��ɂ��ďd�Ȃ�����߂� ---
         for (int i = 0; i < 3; ++i) {
             float overlap = CalculateAxisOverlap(a, b, axesA[i]);
             // ����overlap��0�ȉ��Ȃ�Փ˂Ȃ��Ƃ݂Ȃ��A�����ɏ�����ł��؂�������
             if (overlap <= 0.0f) {
                 return Vector3::zero; // �Փ˂Ȃ��i�@���̓[���x�N�g���j
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


    // Sphere��Box�Ԃ̏Փ˖@�����v�Z
    Vector3 CalculateCollisionNormal(const SphereCollider &sphere, const BoxCollider &box) {
        Vector3 normal = sphere.GetCenter() - box.GetClosestPoint(sphere.GetCenter());
        if (normal.LengthSquared() > 0) {
            return normal.Normalize();
        }
        return Vector3(0, 1, 0); // �f�t�H���g�@��
    }

    // Sphere�Ԃ̏Փ˖@�����v�Z
    Vector3 CalculateCollisionNormal(const SphereCollider &sphereA, const SphereCollider &sphereB) {
        Vector3 normal = sphereB.GetCenter() - sphereA.GetCenter();
        if (normal.LengthSquared() > 0) {
            return normal.Normalize();
        }
        return Vector3(0, 1, 0); // �f�t�H���g�@��
    }

    // ================================
    // Rigidbody �N���X
    // ================================

    void Rigidbody::UpdateForceAndAcceleration() {
        if (velocity.LengthSquared() < 0.0001f && isGround) {
            return; // ����̃t���[���͉������Ȃ�
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

        // �I�N�c���[�Ō����擾
        AABB ownAABB = GetAABB(object);
        sceneOctree->Query(ownAABB, result);

        AsyncHitTest();
        //HitTest();


        // �ՓˏI������
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

        // �g���K�[�I������
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

    //    // ���݂̃I�u�W�F�N�g�̃R���C�_�[���擾
    //    //auto collider = object->GetComponent<Collider>();
    //    if (!collider) return false; // �R���C�_�[���Ȃ��ꍇ�͑����ɏI��

    //    // �Փˌ�⃊�X�g���`�F�b�N
    //    for (auto obj : result) {
    //        if (obj == object) continue; // �������g�̓X�L�b�v

    //        auto otherCollider = obj->GetComponent<Collider>();
    //        if (!otherCollider) continue; // ���̃I�u�W�F�N�g�ɃR���C�_�[���Ȃ���΃X�L�b�v

    //        // �Փ˂��Ă��邩���m�F
    //        if (collider->IsColliding(*otherCollider)) {
    //            // �Փ˖@�����v�Z
    //            Vector3 normal = CalculateCollisionNormal(*collider, *otherCollider);

    //            // �@����������̏ꍇ�iY�������̒l�j��ڒn�Ƃ��Ĕ��f
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

            // �Փ˔���
            if (collider->IsColliding(*otherCollider)) {
                if (collider->isTrigger || otherCollider->isTrigger) {
                    // �g���K�[����
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
                    continue; // �������Z���X�L�b�v
                }

                // �ʏ�̕����Փˏ���
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

                // �����W�����g�p�������x�v�Z
                float restitution = 0.4f;
                velocity = velocity - (1 + restitution) * velocity.Dot(normal) * normal;
            }
        }
    }

    void Rigidbody::AsyncHitTest() {
        thread_local std::vector<Collision> localCollisions; // �X���b�h���[�J���̈ꎞ�f�[�^
        thread_local std::vector<Collision> localTriggers;

        // �������Ŏ��O�ɃN���A�����U�[�u���Ă���
        localCollisions.clear();
        localTriggers.clear();
        // �Ⴆ�� result.size() ���炢�͏Փ˔��肷��\��������̂Ŋm�ۂ��Ă���
        localCollisions.reserve(result.size());
        localTriggers.reserve(result.size());

		isGround = false;

        std::for_each(std::execution::par, result.begin(), result.end(), [&](auto &gameObject) {
            if (gameObject == object) return;

            auto otherCollider = gameObject->GetComponent<Collider>();
            if (!otherCollider) return;

            // �Փ˔���
            if (collider->IsColliding(*otherCollider)) {
                Vector3 contactPoint = CalculateContactPoint(*collider, *otherCollider);
                Vector3 normal = CalculateCollisionNormal(*collider, *otherCollider);
                // �@����������̏ꍇ�iY�������̒l�j��ڒn�Ƃ��Ĕ��f
                if (normal.y > 0.5f) {
                    isGround = true;
                }

                if (collider->isTrigger || otherCollider->isTrigger) {
                    // �g���K�[����
                    Collision triggerCollision = Collision::CreateCollision(otherCollider.get(), gameObject, contactPoint, normal);
                    localTriggers.push_back(triggerCollision);
                }
                else {
                    // �ʏ�Փˏ���
                    Collision collision = Collision::CreateCollision(otherCollider.get(), gameObject, contactPoint, normal);
                    localCollisions.push_back(collision);

                    // �����W�����g�p�������x�v�Z
                    float restitution = 0.4f;
                    velocity = velocity - (1 + restitution) * velocity.Dot(normal) * normal;
                }
            }
            });

        // ���ʂ��}�[�W�i��������j
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
