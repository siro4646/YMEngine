#include "rigidBody.h"

#include "gameFrameWork/gameObject/gameObject.h"
#include "gameFrameWork/gameObject/gameObjectManager.h"


#include "ocTree/ocTree.h"
#include "requiredObject/sceneOcTree/sceneOcTree.h"

#include "utility/inputSystem/keyBoard/keyBoardInput.h"

#include "application/application.h"

#include "gameFrameWork/collider/collider.h"

namespace ym
{

	void Rigidbody::Init()
	{
		//�V�[���̃I�N�c���[���擾
		sceneOctree = object->objectManager->FindGameObjects<SceneOcTree>()[0]->GetSceneOctree();		
	}

	void Rigidbody::FixedUpdate()
	{
		// �͂Ɖ����x���X�V
		UpdateForceAndAcceleration();

		// �Փˌ��o�Ɖ���
		DetectAndResolveCollisions();

		// ���x���X�V
		UpdateVelocity();
		// �ʒu���X�V
		UpdatePosition();

		// �͂����Z�b�g
		force = Vector3(0, 0, 0);
	}

	void Rigidbody::Update()
	{
		if (!collider)
		{
			collider = object->GetComponent<Collider>().get();
		}


	}

	void Rigidbody::Draw()
	{
	}

	void Rigidbody::Uninit()
	{
	}

	void Rigidbody::AddForce(const Vector3 &additionalForce ,const ForceMode mode)
	{
		auto delta = Application::Instance()->GetDeltaTime();
		
		switch (mode)
		{
		case ForceMode::Force:
			force += additionalForce * delta / mass;
			break;
		case ForceMode::Impulse:
			force += additionalForce/mass;
			break;
		case ForceMode::VelocityChange:
			force += additionalForce;
			break;
		case ForceMode::Acceleration:
			force += additionalForce * delta;
			break;
		default:
			break;

		}
		//force += additionalForce;
	}
	}