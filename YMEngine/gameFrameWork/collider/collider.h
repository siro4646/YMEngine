#pragma once

#include "gameFrameWork/components/component.h"
#include "gameFrameWork/gameObject/gameObject.h"
#include "gameFrameWork/gameObject/gameObjectManager.h"
#include "ocTree/ocTree.h"

#include "gameFrameWork/requiredObject/sceneOcTree/sceneOcTree.h"

#include "gameFrameWork/raycast/raycast.h"

namespace ym {
    class Object;
	class BoxCollider;
	class SphereCollider;
    struct Ray;
    struct RayHit;

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
           assert(object != nullptr);
           auto octrees = object->objectManager->FindGameObjects<SceneOcTree>();
           assert(!octrees.empty());
           sceneOctree = octrees[0]->GetSceneOctree();
           sceneOctree->Insert(object);
        }
        void Update()override
        {

        }

		void FixedUpdate()override
		{
            if (!object || !sceneOctree) return; // null ���S�΍�
            sceneOctree->Remove(object);
            sceneOctree->Insert(object);
        }

        void Draw()override
        {}
        virtual void DrawImguiBody() override {}
        void Uninit()override
        {
			sceneOctree->Remove(object);
        }
		inline virtual bool IsCollidingAt(const Vector3 &futurePosition, const Collider &other) = 0;

       inline virtual bool IsColliding(const Collider &other) const = 0;
        // BoxCollider�Ƃ̏Փ˔���i�_�u���f�B�X�p�b�`�p�j
       inline virtual bool CollideWithBox(const BoxCollider &box) const {
            return false; // �f�t�H���g�͑Ή����Ă��Ȃ�
        }
	   inline virtual bool CollideWithSphere(const SphereCollider &sphere) const {
		   return false; // �f�t�H���g�͑Ή����Ă��Ȃ�
	   }
	   virtual bool Raycast(const Ray &ray, float &t, Vector3 &hitPoint) const {
		   return false; // �f�t�H���g�͑Ή����Ă��Ȃ�
	   }

	   // �Փˌ`��̃^�C�v���擾

       virtual ColliderType GetColliderType() const = 0;

        virtual ~Collider() {
			//sceneOctree->Remove(object);
        }
        virtual inline Vector3 GetCenter() const {
            return object->worldTransform.Position; // Transform�̈ʒu���擾
        }

        virtual inline Vector3 GetScale() const {
            return object->worldTransform.Scale*0.5f; // Transform�̃X�P�[�����擾
        }

        inline Vector3 GetRotation() const {
            return object->worldTransform.Rotation; // Transform�̉�]���擾
        }
		inline void SetCenter(const Vector3 &center) {
			object->worldTransform.Position = center; // Transform�̈ʒu��ݒ�
		}

		inline void SetScale(const Vector3 &scale) {
			object->worldTransform.Scale = scale; // Transform�̃X�P�[����ݒ�
		}

		inline void SetRotation(const Vector3 &rotation) {
			object->worldTransform.Rotation = rotation; // Transform�̉�]��ݒ�
		}

		bool isTrigger = false; // �g���K�[���ǂ���

		void OnTriggerEnter(Collision &col) override {
			ym::ConsoleLogRelease("%s��%s�ɐN�����܂���\n", object->name.c_str(), col.object->name.c_str());
		}


    private:
    protected:
		Transform transform; // Transform���
		// �V�[���̃I�N�c���[�ւ̎Q��
		Octree *sceneOctree;
    };

    class BoxCollider : public Collider,public IPropertyProvider
    {

	public:
        Vector3 center = Vector3::zero;
        Vector3 size = Vector3(1, 1, 1); // �f�t�H���g��1x1x1
       
		std::string GetProviderName() const override {
			return "BoxCollider";
		}
		std::vector<PropertyInfo> GetProperties() override {
			std::vector<PropertyInfo> props;
			props.push_back({ "Center.x", &center.x });
			props.push_back({ "Center.y", &center.y });
			props.push_back({ "Center.z", &center.z });
			props.push_back({ "Size.x", &size.x });
			props.push_back({ "Size.y", &size.y });
			props.push_back({ "Size.z", &size.z });
			return props;
		}

		using Collider::Collider;     
        void Init()override;
        void Update()override;
		void FixedUpdate()override;
        void Draw()override;
        void DrawImguiBody() override {
            ImGui::Text("Box Collider");
            ImGui::Checkbox("Is Trigger", &isTrigger);

            ImGui::DragFloat3("Center", &center.x, 0.01f);
            auto scale = transform.Scale;
			auto position = transform.Position;
            //disp
			ImGui::DragFloat3("Scale", &scale.x, 0.01f, 0.0f, 10.0f, "%.2f");
			ImGui::DragFloat3("Position", &position.x, 0.01f, -10.0f, 10.0f, "%.2f");
        }
		Vector3 GetCenter() const override {
			return object->worldTransform.Position + center; // Transform�̈ʒu�Ƀ��[�J�����S�����Z
		}

        Vector3 GetScale() const override {
            return transform.Scale*0.5f; // Transform�̃X�P�[���Ƀ��[�J���T�C�Y�����Z

        }
        void Uninit()override;

        const char *GetName() const override {
            return "BoxCollider";
        }

		bool IsCollidingAt(const Vector3 &futurePosition, const Collider &other)  override;
		bool IsColliding(const Collider &other) const override;
        bool CollideWithBox(const BoxCollider &box) const override;
        bool CollideWithSphere(const SphereCollider &sphere) const override;
		std::array<Vector3, 3> GetLocalAxes() const;
        bool OBBIntersects(const BoxCollider &a, const BoxCollider &b) const;
        Vector3 GetClosestPoint(const Vector3 &point)const;

        //inline Vector3 

        bool Raycast(const Ray &ray, float &t, Vector3 &hitPoint)const override;

		inline ColliderType GetColliderType() const override {
			return ColliderType::Box;
		}

		~BoxCollider() {
			//sceneOctree->Remove(object);
        }

    };
	REGISTER_COMPONENT(BoxCollider);
	class SphereCollider : public Collider, public IPropertyProvider
    {
    public:
        Vector3 center = Vector3::zero;
        float radiusOffset = 0;    // �X�t�B�A�̔��a

		std::string GetProviderName() const override {
			return "SphereCollider";
		}
        std::vector<PropertyInfo> GetProperties() override {
			std::vector<PropertyInfo> props;
			props.push_back({ "Center.x", &center.x });
			props.push_back({ "Center.y", &center.y });
			props.push_back({ "Center.z", &center.z });
			props.push_back({ "Radius Offset", &radiusOffset });
			return props;
        }

        using Collider::Collider; // ���N���X�̃R���X�g���N�^���p��

        void Init() override;
        void Update() override;
        void FixedUpdate()override;
        void Draw() override;
        void DrawImguiBody() override {
            ImGui::Text("Sphere Collider");
            ImGui::Checkbox("Is Trigger", &isTrigger);
            ImGui::DragFloat3("Center", &center.x, 0.01f);

			float baseRadius = object->worldTransform.Scale.y * 0.5f + radiusOffset;
			ImGui::DragFloat("Radius Offset", &baseRadius, 0.01f, 0.0f, 10.0f, "%.2f");
			radiusOffset = baseRadius - object->worldTransform.Scale.y * 0.5f; // �I�u�W�F�N�g�̃X�P�[�����l�����Ĕ��a�I�t�Z�b�g��ݒ�
        }

        Vector3 GetCenter() const override {
            return object->worldTransform.Position + center;
        }

        Vector3 GetScale() const override {
            float r = object->worldTransform.Scale.y * 0.5f + radiusOffset;
            return Vector3(r, r, r);
        }
		void Uninit()override;
		const char *GetName() const override {
			return "SphereCollider";
		}
        bool IsCollidingAt(const Vector3 &futurePosition, const Collider &other)  override;
        bool IsColliding(const Collider &other) const override;
        bool CollideWithSphere(const SphereCollider &sphere) const override;
        bool CollideWithBox(const BoxCollider &box) const override;

      
        bool Raycast(const Ray &ray, float &t, Vector3 &hitPoint)const override;


		inline ColliderType GetColliderType() const override {
			return ColliderType::Sphere;
		}

        ~SphereCollider() {
			//sceneOctree->Remove(object);
        }
    };
	REGISTER_COMPONENT(SphereCollider);

}