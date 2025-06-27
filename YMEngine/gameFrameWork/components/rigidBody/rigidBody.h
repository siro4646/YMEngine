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

	class Rigidbody : public Component, public IPropertyProvider
	{
	public:
		std::string GetProviderName() const override { return "Rigidbody"; }
		std::vector<PropertyInfo> GetProperties() override
		{
			return {
				{ "Mass", &mass },
				{ "Drag", &drag },
				{ "Angular Drag", &angularDrag },
				//{ "Use Gravity", &useGravity },
				//{ "Is Kinematic", &isKinematic },
				{ "Interpolation", reinterpret_cast<float *>(&interpolation) }, // enum �� float �|�C���^�ɃL���X�g
				{ "Restitution", &restitution }
			};
		}

		using Component::Component;

		void Init()override;
		void FixedUpdate() override;
		void Update()override;
		void Draw()override;
		void DrawImguiBody() override;
		void Uninit()override;
		const char *GetName() const override { return "Rigidbody"; }

		void AddForce(const Vector3 &additionalForce,const ForceMode mode = ForceMode::Force);

		void RegisterHitObject(Object *hitObject);

		bool CheckHitObject(Object *hitObject);

	private:

		void UpdateForceAndAcceleration();
		void DetectAndResolveCollisions();
		void UpdatePosition();
		void UpdateVelocity();		
		//bool IsOnGround();	
		void ResolveCollision(Object *hitTarget,Vector3 normal);

		void ResolveCollision(Object *hitTarget, Vector3 normal, Vector3 predictedPosition);

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

		float restitution = 0.3f;//�����W��
		

		std::vector<Object *> result;//�I�N�c���[�����ƂɏՓ˂���\��������I�u�W�F�N�g�̊i�[

		virtual ~Rigidbody() {}

		Octree *sceneOctree;
	private:

		Collider *collider=nullptr;

		std::unordered_set<Object *> currentCollisions; // ���݂̏Փˑ���
		std::unordered_set<Object *> previousCollisions; // �O�t���[���̏Փˑ���

		std::unordered_set<Object *> previousTriggers; // �O�t���[���̃g���K�[
		std::unordered_set<Object *> currentTriggers;  // ���t���[���̃g���K�[

		//���̃t���[���ŏՓ˂����I�u�W�F�N�g
		std::unordered_set<Object *> hitObjects;

	};
	REGISTER_COMPONENT(Rigidbody);
}