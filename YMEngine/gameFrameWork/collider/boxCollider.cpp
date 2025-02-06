#include "collider/collider.h"
#include "gameFrameWork/gameObject/gameObject.h"

#include "gameFrameWork/components/objLoader/objLoader.h"

#include "gameFrameWork/material/debugMaterial.h"


namespace ym
{
	

	void BoxCollider::Init()
	{
		const wchar_t *modelFile = L"asset/model/cube.obj";
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
		
		//�I�N�c���[�ɒǉ�
		//sceneOctree->Insert(object);
	}

	void BoxCollider::Update()
	{
		
	}

	void BoxCollider::FixedUpdate()
	{
		//�������삪�d��������l����
		// �I�N�c���[����폜���čēo�^
		sceneOctree->Remove(object);
		sceneOctree->Insert(object);
	}

	void BoxCollider::Draw()
	{

	}

	void BoxCollider::Uninit()
	{
		Collider::Uninit();

	}

	bool BoxCollider::IsCollidingAt(const Vector3 &futurePosition, const Collider &other) 
	{
		Vector3 originalPosition = GetCenter(); // ���݂̈ʒu
		SetCenter(futurePosition); // ���̈ʒu�Ɉړ�

		bool result = other.IsColliding(*this); // �Փ˔���

		SetCenter(originalPosition); // ���̈ʒu�ɖ߂�
		return result;
	}

	/**
	 * ���[�J�����W�n�̊e�����v�Z����
	 * @return �e���̕����x�N�g�����i�[�����z��
	 */
	std::array<Vector3, 3> BoxCollider::GetLocalAxes() const
	{
		auto rotation = GetRotation(); // ��]�p�x
		// �x�����W�A���ɕϊ�
		float radX = rotation.x * M_PI / 180.0f;
		float radY = rotation.y * M_PI / 180.0f;
		float radZ = rotation.z * M_PI / 180.0f;

		float sinX = sinf(radX), cosX = cosf(radX);
		float sinY = sinf(radY), cosY = cosf(radY);
		float sinZ = sinf(radZ), cosZ = cosf(radZ);

		// ���[�J��X���i1,0,0����]���������ʁj
		Vector3 localX(
			cosY * cosZ,
			cosY * sinZ,
			-sinY
		);

		// ���[�J��Y���i0,1,0����]���������ʁj
		Vector3 localY(
			sinX * sinY * cosZ - cosX * sinZ,
			sinX * sinY * sinZ + cosX * cosZ,
			sinX * cosY
		);

		// ���[�J��Z���i0,0,1����]���������ʁj
		Vector3 localZ(
			cosX * sinY * cosZ + sinX * sinZ,
			cosX * sinY * sinZ - sinX * cosZ,
			cosX * cosY
		);

		return { localX, localY, localZ };
	}

	bool BoxCollider::IsColliding(const Collider &other) const
	{
		return other.CollideWithBox(*this);		
	}
	bool BoxCollider::CollideWithBox(const BoxCollider &box) const
	{
		return OBBIntersects(*this, box); // OBB���m�̏Փ˔���
	}
	bool BoxCollider::CollideWithSphere(const SphereCollider &sphere) const
	{
		return sphere.CollideWithBox(*this);
	}
	/**
 * OBB���m�̏Փ˔���i�������藝���g�p�j
 */
	bool BoxCollider::OBBIntersects(const BoxCollider &a, const BoxCollider &b) const {
		// OBB�̊e���i���[�J�����W�n�j���擾
		auto aAxes = a.GetLocalAxes(); // A�̃��[�J���� (X, Y, Z)
		auto aCenter = a.GetCenter(); // A�̒��S���W
		auto aScale = a.GetScale(); // A�̃X�P�[��
		auto bAxes = b.GetLocalAxes(); // B�̃��[�J���� (X, Y, Z)
		auto bCenter = b.GetCenter(); // B�̒��S���W
		auto bScale = b.GetScale(); // B�̃X�P�[��


		// ���S�Ԃ̃x�N�g��
		Vector3 T = bCenter - aCenter;

		// A�̃��[�J����Ԃɕϊ�
		T = Vector3(T.Dot(aAxes[0]), T.Dot(aAxes[1]), T.Dot(aAxes[2]));

		// A�̎��𕪗����Ƃ��Ĕ���
		for (int i = 0; i < 3; ++i) {
			float RA = aScale[i]; // A�̎������̔��a
			float RB = bScale.x * std::abs(bAxes[0].Dot(aAxes[i])) +
				bScale.y * std::abs(bAxes[1].Dot(aAxes[i])) +
				bScale.z * std::abs(bAxes[2].Dot(aAxes[i]));

			if (std::abs(T[i]) > RA + RB) return false; // ����������������
		}

		// B�̎��𕪗����Ƃ��Ĕ���
		for (int i = 0; i < 3; ++i) {
			float RA = aScale.x * std::abs(aAxes[0].Dot(bAxes[i])) +
				aScale.y * std::abs(aAxes[1].Dot(bAxes[i])) +
				aScale.z * std::abs(aAxes[2].Dot(bAxes[i]));
			float RB = bScale[i]; // B�̎������̔��a

			if (std::abs(T.Dot(bAxes[i])) > RA + RB) return false; // ����������������
		}
		// �N���X���ɂ�镪�����𔻒�
		for (int i = 0; i < 3; ++i) {        // A�̎�
			for (int j = 0; j < 3; ++j) {    // B�̎�
				// �N���X�� = aAxes[i] �~ bAxes[j]
				Vector3 crossAxis = aAxes[i].Cross(bAxes[j]);
				if (crossAxis.LengthSquared() < 1e-6f) continue; // �������[���̏ꍇ�̓X�L�b�v

				// A��B���N���X���ɓ��e
				float RA = aScale.x * std::abs(aAxes[0].Dot(crossAxis)) +
					aScale.y * std::abs(aAxes[1].Dot(crossAxis)) +
					aScale.z * std::abs(aAxes[2].Dot(crossAxis));

				float RB = bScale.x * std::abs(bAxes[0].Dot(crossAxis)) +
					bScale.y * std::abs(bAxes[1].Dot(crossAxis)) +
					bScale.z * std::abs(bAxes[2].Dot(crossAxis));

				float distance = std::abs(T.Dot(crossAxis));

				if (distance > RA + RB) return false; // ����������������
			}
		}

		return true; // ��������������Ȃ������ꍇ�A�Փ˂��Ă���
	}

	Vector3 BoxCollider::GetClosestPoint(const Vector3 &point)const
	{
		Vector3 center = GetCenter(); // �{�b�N�X�̒��S���W
		Vector3 localPoint = point - center; // �{�b�N�X�̃��[�J�����W�n�ɕϊ�
		Vector3 closestPoint = Vector3::zero;
		Vector3 scale = GetScale(); // �{�b�N�X�̃X�P�[��

		auto axes = GetLocalAxes(); // ���[�J����
		for (int i = 0; i < 3; ++i) {
			float projection = localPoint.Dot(axes[i]);
			projection = std::max(-scale[i], std::min(projection, scale[i]));
			closestPoint += projection * axes[i];
		}

		return closestPoint + center; // ���[���h���W�ɖ߂�
	}

}