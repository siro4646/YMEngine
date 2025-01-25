#pragma once
#include "gameFrameWork/components/component.h"
namespace ym
{
	class Collider;

#define GRAVITY (9.8f)//重力

	enum class RB_INTERPOLATION
	{
		None,//補間しない
		Interpolate,//前フレームの Transform にもとづいて Transform をスムージング
		Extrapolate//次のフレームの Transform を予測して Transform をスムージング
	};

	struct Constraints
	{
		bool freezePositionX = false;
		bool freezePositionY = false;
		bool freezePositionZ = false;
		bool freezeRotationX = false;
		bool freezeRotationY = false;
		bool freezeRotationZ = false;

	};

	class Octree;

	enum class ForceMode
	{
		Force,//デルタタイムを計算する
		Impulse,//デルタタイムを計算しない
		VelocityChange,//デルタタイムを計算せず,Massを考慮しない
		Acceleration//デルタタイムを計算し,Massを考慮しない
	};

	class Rigidbody : public Component
	{
	public:
		using Component::Component;

		void Init()override;
		void FixedUpdate() override;
		void Update()override;
		void Draw()override;
		void Uninit()override;

		void AddForce(const Vector3 &additionalForce,const ForceMode mode = ForceMode::Force);


	private:

		void UpdateForceAndAcceleration();
		void DetectAndResolveCollisions();
		void UpdatePosition();
		void UpdateVelocity();		
		//bool IsOnGround();	

		void  HitTest();
		void AsyncHitTest();

	public:
		float mass = 1.0f;
		float drag = 0.1f;
		float angularDrag = 0.05f;
		bool useGravity = true;
		bool isKinematic = false;
		bool isGround = false;
		RB_INTERPOLATION interpolation = RB_INTERPOLATION::None;

		Constraints constraints;

		//速度
		Vector3 velocity;
		//角速度
		Vector3 angularVelocity;
		//力
		Vector3 force;

		

		std::vector<Object *> result;//オクツリーをもとに衝突する可能性があるオブジェクトの格納

		virtual ~Rigidbody() {}

		Octree *sceneOctree;
	private:

		Collider *collider=nullptr;

		std::unordered_set<Object *> currentCollisions; // 現在の衝突相手
		std::unordered_set<Object *> previousCollisions; // 前フレームの衝突相手

		std::unordered_set<Object *> previousTriggers; // 前フレームのトリガー
		std::unordered_set<Object *> currentTriggers;  // 現フレームのトリガー
	};

}