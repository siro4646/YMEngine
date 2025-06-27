#pragma once

#include <memory>
#include <string>

#include "gameObject/gameObject.h"
#include "ComponentRegistry.h"

namespace ym
{
	class Object;
	class Device;
	class CommandList;
	class Collider;
	class Rigidbody;
	class Transform;

	class Collision
	{
	public:
		Collider *collider{};
		Object *object{};
		Rigidbody *rigidbody{};
		Transform *transform{};
		Vector3 contactPoint{};
		Vector3 normal{};

		Collision(Collider *collider = nullptr, Object *object = nullptr,
			Rigidbody *rigidbody = nullptr, Transform *transform = nullptr,
			const Vector3 &contactPoint = Vector3::zero, const Vector3 &normal = Vector3::zero)
			: collider(collider), object(object), rigidbody(rigidbody),
			transform(transform), contactPoint(contactPoint), normal(normal) {}

		~Collision() {}


		static Collision CreateCollision(Collider *otherCollider, Object *otherObject, Vector3 contactPoint, Vector3 normal);
	};

	class Component
	{
	protected:
		std::shared_ptr<Device> pDevice_;
		std::shared_ptr<CommandList> pCmdList_;

	public:
		Object *object;
		bool isEnabled = true; // <- UIからON/OFF切り替え可能な実行フラグ

		bool isFailed = false; // <- 初期化失敗フラグ

		Component() = delete;
		Component(Object *object);
		virtual ~Component() = default;

		virtual void Init() = 0;
		virtual void FixedUpdate() = 0;
		virtual void Update() = 0;
		virtual void Draw() = 0;
		virtual void DrawImgui();            // ImGui統一呼び出し
		virtual void DrawImguiBody() {}      // 派生クラスで実装可能
		virtual void Uninit() = 0;

		virtual const char *GetName() const = 0;

		// 衝突イベント（仮想関数）
		inline virtual void OnCollisionEnter(Collision &col) {}
		inline virtual void OnCollisionExit(Collision &col) {}
		inline virtual void OnCollisionStay(Collision &col) {}

		// トリガーイベント（仮想関数）
		inline virtual void OnTriggerEnter(Collision &col) {}
		inline virtual void OnTriggerExit(Collision &col) {}
		inline virtual void OnTriggerStay(Collision &col) {}

		template <typename T>  std::shared_ptr<T> GetComponent()
		{
			return object->GetComponent<T>();
		}
		template <typename T>  std::shared_ptr<T> AddComponent()
		{
			return object->AddComponent<T>();
		}
		template <typename T>  void RemoveComponent()
		{
			object->RemoveComponent<T>();
		}
	};
}
