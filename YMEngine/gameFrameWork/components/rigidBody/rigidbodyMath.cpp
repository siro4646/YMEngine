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
		hitObjects.clear();

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

        // �ł��[���߂荞�ݎ������
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
        // �ʒu�␳�̓K�p�i�ߕ␳��h���j

        float correctionFactor = 1.05f; // 100% ���ƐU�����邽�ߔ�����
        float epsilon = 0.001f; // �����Ȍ��Ԃ��J����
        Vector3 correction = normal * (minPenetration * correctionFactor + epsilon);



            // ���ʔ���l���i�y��������艟���߂����j
            float totalMass = mass + hitRb->mass;
            float massRatioA = (totalMass > 0) ? (hitRb->mass / totalMass) : 0.5f;
            float massRatioB = 1.0f - massRatioA;

            object->localTransform.Position += correction * massRatioA;
            hitTarget->localTransform.Position -= correction * massRatioB;

            // �����W���̌���i���҂̕��ς����j
            float restitution = std::min(this->restitution, hitRb->restitution);

            // ���x�̕����i�@�������Ɛڐ������j
            Vector3 relativeVelocity = velocity - hitRb->velocity;
            float normalVelocity = relativeVelocity.Dot(normal);

            float inverseMassA = (mass > 0) ? (1 / mass) : 0;
            float inverseMassB = (hitRb->mass > 0) ? (1 / hitRb->mass) : 0;

            float impulse = -(1 + restitution) * normalVelocity;
            impulse /= (inverseMassA + inverseMassB);

            if (normalVelocity < 0) { // �Փ˂��Ă���ꍇ�̂ݓK�p
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
        if (!hitRb) return; // Rigidbody ���Ȃ��ꍇ�͉������Ȃ�
        if (hitRb->CheckHitObject(object)) return; // ���łɏ����ς݂Ȃ�X�L�b�v

        // �X�P�[���̎擾
        Vector3 scaleA = this->collider->GetScale();
        Vector3 scaleB = hitRb->collider->GetScale();
        Vector3 radSum = (scaleA + scaleB) * 0.5f;
        Vector3 distance = hitTarget->worldTransform.Position - predictedPosition;
        Vector3 penetration = radSum - Vector3(abs(distance.x), abs(distance.y), abs(distance.z));

        // **�ő�� penetrationDepth ���擾**
        float penetrationDepth = std::max({ penetration.x, penetration.y, penetration.z });
        if (penetrationDepth <= 0.0f) return; // �Փ˂��Ă��Ȃ��ꍇ�̓X�L�b�v

        // **�ʒu�␳�̓K�p**
        float correctionFactor = 1.05f; // �ߕ␳��h��
        float epsilon = 0.001f; // �����Ȍ��Ԃ��J����
        Vector3 correction = normal * (penetrationDepth * correctionFactor + epsilon);

        // **���ʔ���l��**
        float totalMass = mass + hitRb->mass;
        float massRatioA = (totalMass > 0) ? (hitRb->mass / totalMass) : 0.5f;
        float massRatioB = 1.0f - massRatioA;

        // **�����ʒu��K�p**
        Vector3 correctedPosition = predictedPosition + correction * massRatioA;

        // **�I�u�W�F�N�g�̈ʒu���X�V**
        object->localTransform.Position = correctedPosition;

        // **�Փ˃��X�g�ɓo�^**
        RegisterHitObject(hitTarget);
        hitRb->RegisterHitObject(object);
    }



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
               // float restitution = 0.4f;
                velocity = velocity - (1 + restitution) * velocity.Dot(normal) * normal;
            }
        }
    }

    void Rigidbody::AsyncHitTest()
    {
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

                    // obj->worldTransform.Position -= velocity;

                    // ResolveCollision(gameObject, normal);

                     // �����W�����g�p�������x�v�Z
                    // float restitution = 0.4f;
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
                    //�g���K�[����
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
