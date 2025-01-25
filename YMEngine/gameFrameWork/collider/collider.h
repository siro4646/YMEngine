#pragma once

#include "gameFrameWork/components/component.h"
#include "gameFrameWork/gameObject/gameObject.h"
#include "gameFrameWork/gameObject/gameObjectManager.h"
#include "ocTree/ocTree.h"

#include "gameFrameWork/requiredObject/sceneOcTree/sceneOcTree.h"

namespace ym {
    class Object;
	class BoxCollider;
	class SphereCollider;

    enum class ColliderType {
        Box,
        Sphere
        // ���̃^�C�v������Βǉ�
    };

    // SAT���茋�ʃL���b�V���\����
    struct SATCache {
        std::array<Vector3, 6> axes;      // A��3�� + B��3��
        std::array<Vector3, 9> crossAxes; // A�~B��9��
        Vector3 T;                        // ���S�Ԃ̃x�N�g��
    };

  

    // �Փˌ`��̊�{�N���X
	class Collider : public Component
    {
    public:
		using Component::Component;

       inline void Init()override
        {
           sceneOctree = object->objectManager->FindGameObjects<SceneOcTree>()[0]->GetSceneOctree();
		   sceneOctree->Insert(object);
        }
        void Update()override
        {}

		void FixedUpdate()override
		{

        }

        void Draw()override
        {}
        void Uninit()override
        {
			sceneOctree->Remove(object);
        }


       inline virtual bool IsColliding(const Collider &other) const = 0;
        // BoxCollider�Ƃ̏Փ˔���i�_�u���f�B�X�p�b�`�p�j
       inline virtual bool CollideWithBox(const BoxCollider &box) const {
            return false; // �f�t�H���g�͑Ή����Ă��Ȃ�
        }
	   inline virtual bool CollideWithSphere(const SphereCollider &sphere) const {
		   return false; // �f�t�H���g�͑Ή����Ă��Ȃ�
	   }

       virtual ColliderType GetColliderType() const = 0;

        virtual ~Collider() {
			//sceneOctree->Remove(object);
        }
        inline Vector3 GetCenter() const {
            return object->worldTransform.Position; // Transform�̈ʒu���擾
        }

        inline Vector3 GetScale() const {
            return object->worldTransform.Scale*0.5f; // Transform�̃X�P�[�����擾
        }

        inline Vector3 GetRotation() const {
            return object->worldTransform.Rotation; // Transform�̉�]���擾
        }

		bool isTrigger = false; // �g���K�[���ǂ���

		void OnTriggerEnter(Collision &col) override {
			ym::ConsoleLogRelease("%s��%s�ɐN�����܂���\n", object->name.c_str(), col.object->name.c_str());
		}


    private:
    protected:
       
		Octree *sceneOctree;
    };

    class BoxCollider : public Collider
    {

	public:
       
		using Collider::Collider;       
        void Init()override;
        void Update()override;
		void FixedUpdate()override;
        void Draw()override;
        void Uninit()override;
		bool IsColliding(const Collider &other) const override;
        bool CollideWithBox(const BoxCollider &box) const override;
        bool CollideWithSphere(const SphereCollider &sphere) const override;
		std::array<Vector3, 3> GetLocalAxes() const;
        bool OBBIntersects(const BoxCollider &a, const BoxCollider &b) const;
        Vector3 GetClosestPoint(const Vector3 &point)const;

        //inline Vector3 

		inline ColliderType GetColliderType() const override {
			return ColliderType::Box;
		}

		~BoxCollider() {
			//sceneOctree->Remove(object);
        }
    };

    class SphereCollider : public Collider {
    public:
        
        float radius;    // �X�t�B�A�̔��a

        using Collider::Collider; // ���N���X�̃R���X�g���N�^���p��

        void Init() override;
        void Update() override;
        void FixedUpdate()override;
        void Draw() override;
		void Uninit()override;

        bool IsColliding(const Collider &other) const override;
        bool CollideWithSphere(const SphereCollider &sphere) const override;
        bool CollideWithBox(const BoxCollider &box) const override;

      


		inline ColliderType GetColliderType() const override {
			return ColliderType::Sphere;
		}

        ~SphereCollider() {
			//sceneOctree->Remove(object);
        }
    };

}