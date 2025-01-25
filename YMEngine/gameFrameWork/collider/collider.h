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
        // BoxColliderとの衝突判定（ダブルディスパッチ用）
       inline virtual bool CollideWithBox(const BoxCollider &box) const {
            return false; // デフォルトは対応していない
        }
	   inline virtual bool CollideWithSphere(const SphereCollider &sphere) const {
		   return false; // デフォルトは対応していない
	   }

       virtual ColliderType GetColliderType() const = 0;

        virtual ~Collider() {
			//sceneOctree->Remove(object);
        }
        inline Vector3 GetCenter() const {
            return object->worldTransform.Position; // Transformの位置を取得
        }

        inline Vector3 GetScale() const {
            return object->worldTransform.Scale*0.5f; // Transformのスケールを取得
        }

        inline Vector3 GetRotation() const {
            return object->worldTransform.Rotation; // Transformの回転を取得
        }

		bool isTrigger = false; // トリガーかどうか

		void OnTriggerEnter(Collision &col) override {
			ym::ConsoleLogRelease("%sが%sに侵入しました\n", object->name.c_str(), col.object->name.c_str());
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
        
        float radius;    // スフィアの半径

        using Collider::Collider; // 基底クラスのコンストラクタを継承

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