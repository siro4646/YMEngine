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
        // 他のタイプがあれば追加
    };

    // SAT判定結果キャッシュ構造体
    struct SATCache {
        std::array<Vector3, 6> axes;      // Aの3軸 + Bの3軸
        std::array<Vector3, 9> crossAxes; // A×Bの9軸
        Vector3 T;                        // 中心間のベクトル
    };

  

    // 衝突形状の基本クラス
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
            if (!object || !sceneOctree) return; // null 安全対策
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
        // BoxColliderとの衝突判定（ダブルディスパッチ用）
       inline virtual bool CollideWithBox(const BoxCollider &box) const {
            return false; // デフォルトは対応していない
        }
	   inline virtual bool CollideWithSphere(const SphereCollider &sphere) const {
		   return false; // デフォルトは対応していない
	   }
	   virtual bool Raycast(const Ray &ray, float &t, Vector3 &hitPoint) const {
		   return false; // デフォルトは対応していない
	   }

	   // 衝突形状のタイプを取得

       virtual ColliderType GetColliderType() const = 0;

        virtual ~Collider() {
			//sceneOctree->Remove(object);
        }
        virtual inline Vector3 GetCenter() const {
            return object->worldTransform.Position; // Transformの位置を取得
        }

        virtual inline Vector3 GetScale() const {
            return object->worldTransform.Scale*0.5f; // Transformのスケールを取得
        }

        inline Vector3 GetRotation() const {
            return object->worldTransform.Rotation; // Transformの回転を取得
        }
		inline void SetCenter(const Vector3 &center) {
			object->worldTransform.Position = center; // Transformの位置を設定
		}

		inline void SetScale(const Vector3 &scale) {
			object->worldTransform.Scale = scale; // Transformのスケールを設定
		}

		inline void SetRotation(const Vector3 &rotation) {
			object->worldTransform.Rotation = rotation; // Transformの回転を設定
		}

		bool isTrigger = false; // トリガーかどうか

		void OnTriggerEnter(Collision &col) override {
			ym::ConsoleLogRelease("%sが%sに侵入しました\n", object->name.c_str(), col.object->name.c_str());
		}


    private:
    protected:
		Transform transform; // Transform情報
		// シーンのオクツリーへの参照
		Octree *sceneOctree;
    };

    class BoxCollider : public Collider,public IPropertyProvider
    {

	public:
        Vector3 center = Vector3::zero;
        Vector3 size = Vector3(1, 1, 1); // デフォルトは1x1x1
       
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
			return object->worldTransform.Position + center; // Transformの位置にローカル中心を加算
		}

        Vector3 GetScale() const override {
            return transform.Scale*0.5f; // Transformのスケールにローカルサイズを加算

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
        float radiusOffset = 0;    // スフィアの半径

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

        using Collider::Collider; // 基底クラスのコンストラクタを継承

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
			radiusOffset = baseRadius - object->worldTransform.Scale.y * 0.5f; // オブジェクトのスケールを考慮して半径オフセットを設定
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