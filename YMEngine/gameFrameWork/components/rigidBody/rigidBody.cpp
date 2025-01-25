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
		//シーンのオクツリーを取得
		sceneOctree = object->objectManager->FindGameObjects<SceneOcTree>()[0]->GetSceneOctree();		
	}

	void Rigidbody::FixedUpdate()
	{
		// 力と加速度を更新
		UpdateForceAndAcceleration();

		// 衝突検出と解決
		DetectAndResolveCollisions();

		// 速度を更新
		UpdateVelocity();
		// 位置を更新
		UpdatePosition();

		// 力をリセット
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