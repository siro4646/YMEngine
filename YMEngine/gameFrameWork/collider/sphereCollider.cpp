#include "collider.h"

#include "gameFrameWork/gameObject/gameObject.h"

#include "gameFrameWork/components/objLoader/objLoader.h"

#include "gameFrameWork/material/debugMaterial.h"

namespace ym
{

	void SphereCollider::Init()
	{
		const wchar_t *modelFile = L"asset/model/sphere_smooth.obj";
		//material->Init();

		int flag = 0;
		flag |= ModelSetting::InverseV;
		//flag |= ModelSetting::InverseU;
		flag |= ModelSetting::AdjustCenter;
		flag |= ModelSetting::AdjustScale;
		ImportSettings importSetting = // ���ꎩ�͎̂���̓ǂݍ��ݐݒ�\����
		{
			modelFile,
			flag
		};

		auto objLoader = object->AddComponent<OBJLoader>().get();
		auto meshes = objLoader->Load(importSetting);
		int count = 0;
		for (auto &mesh : meshes)
		{
			auto debugMaterial = std::make_shared<DebugMaterial>();
			//debugMaterial->SetMainTex("asset/texture/white.dds");
			objLoader->SetMaterial(debugMaterial, count);
			count++;
		}		
		Collider::Init();




	}
	void SphereCollider::Update()
	{
		// �g�����X�t�H�[������X�t�B�A�̏����X�V
		auto transform = object->worldTransform;		
		this->radius = transform.Scale.x * 0.5f; // ���a
	}
	void SphereCollider::FixedUpdate()
	{
		sceneOctree->Remove(object);
		sceneOctree->Insert(object);
	}
	bool SphereCollider::IsColliding(const Collider &other) const
	{
		return other.CollideWithSphere(*this);
	}
	bool SphereCollider::CollideWithSphere(const SphereCollider &sphere) const
	{

		auto center = GetCenter();
		auto sphereCenter = sphere.GetCenter();

		float distanceSquared = (center - sphereCenter).LengthSquared();
		float radiusSum = radius + sphere.radius;

		// ���S�Ԃ̋��������a�̍��v�ȉ��Ȃ�Փ�
		return distanceSquared <= radiusSum * radiusSum;
	}
	bool SphereCollider::CollideWithBox(const BoxCollider &box) const
	{
		auto center = GetCenter();
		auto boxScale = box.GetScale();



		// �X�t�B�A�̒��S���{�b�N�X�̃��[�J����Ԃɕϊ�
		Vector3 sphereCenterInLocal = center - box.GetCenter();

		// ���[�J����Ԃōŋߐړ_�����߂�
		Vector3 closestPoint = Vector3::zero;
		auto axes = box.GetLocalAxes(); // OBB�̃��[�J���� (X, Y, Z)

		for (int i = 0; i < 3; ++i) {
			float projection = sphereCenterInLocal.Dot(axes[i]);
			// �{�b�N�X�̃X�P�[�����ɐ���
			projection = std::clamp(projection, -boxScale[i], boxScale[i]);
			// �ŋߐړ_�����[�J����Ԃŋ��߂�
			closestPoint += projection * axes[i];
		}

		// �X�t�B�A���S�ƍŋߐړ_�Ԃ̋����̕���
		Vector3 distanceVector = sphereCenterInLocal - closestPoint;
		float distanceSquared = distanceVector.LengthSquared();

		

		// �Փ˔���i�����̕����Ŕ�r�j
		return distanceSquared <= (radius * radius);
	}


	void SphereCollider::Draw()
	{
		// �f�o�b�O�p�̕`�揈���i���C���t���[���̃X�t�B�A�Ȃǁj
	}
	void SphereCollider::Uninit()
	{
		Collider::Uninit();
	}
}