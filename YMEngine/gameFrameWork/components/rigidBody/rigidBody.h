#pragma once
#include "gameFrameWork/components/component.h"
namespace ym
{
	class Collider;

#define GRAVITY (9.8f)//�d��

	enum class RB_INTERPOLATION
	{
		None,//��Ԃ��Ȃ�
		Interpolate,//�O�t���[���� Transform �ɂ��ƂÂ��� Transform ���X���[�W���O
		Extrapolate//���̃t���[���� Transform ��\������ Transform ���X���[�W���O
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
		Force,//�f���^�^�C�����v�Z����
		Impulse,//�f���^�^�C�����v�Z���Ȃ�
		VelocityChange,//�f���^�^�C�����v�Z����,Mass���l�����Ȃ�
		Acceleration//�f���^�^�C�����v�Z��,Mass���l�����Ȃ�
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

		//���x
		Vector3 velocity;
		//�p���x
		Vector3 angularVelocity;
		//��
		Vector3 force;

		

		std::vector<Object *> result;//�I�N�c���[�����ƂɏՓ˂���\��������I�u�W�F�N�g�̊i�[

		virtual ~Rigidbody() {}

		Octree *sceneOctree;
	private:

		Collider *collider=nullptr;

		std::unordered_set<Object *> currentCollisions; // ���݂̏Փˑ���
		std::unordered_set<Object *> previousCollisions; // �O�t���[���̏Փˑ���

		std::unordered_set<Object *> previousTriggers; // �O�t���[���̃g���K�[
		std::unordered_set<Object *> currentTriggers;  // ���t���[���̃g���K�[
	};

}