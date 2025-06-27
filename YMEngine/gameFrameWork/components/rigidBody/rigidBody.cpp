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

	void Rigidbody::DrawImguiBody()
	{
		ImGui::Text("Rigidbody");

		ImGui::Checkbox("Use Gravity", &useGravity);
		ImGui::Checkbox("Is Kinematic", &isKinematic);

		ImGui::DragFloat("Mass", &mass, 0.1f, 0.0f, 100.0f);
		ImGui::DragFloat("Drag", &drag, 0.01f, 0.0f, 10.0f);
		ImGui::DragFloat("Angular Drag", &angularDrag, 0.01f, 0.0f, 10.0f);
		ImGui::DragFloat("Restitution", &restitution, 0.01f, 0.0f, 1.0f);

		ImGui::Separator();
		ImGui::Text("Velocity");
		ImGui::DragFloat3("Linear", &velocity.x, 0.01f);
		ImGui::DragFloat3("Angular", &angularVelocity.x, 0.01f);

		ImGui::Separator();
		ImGui::Text("Constraints");
		ImGui::Checkbox("Freeze Pos X", &constraints.freezePositionX);
		ImGui::Checkbox("Freeze Pos Y", &constraints.freezePositionY);
		ImGui::Checkbox("Freeze Pos Z", &constraints.freezePositionZ);
		ImGui::Checkbox("Freeze Rot X", &constraints.freezeRotationX);
		ImGui::Checkbox("Freeze Rot Y", &constraints.freezeRotationY);
		ImGui::Checkbox("Freeze Rot Z", &constraints.freezeRotationZ);

		ImGui::Separator();
		ImGui::Text("Interpolation");
		static const char *interpItems[] = { "None", "Interpolate", "Extrapolate" };
		int current = static_cast<int>(interpolation);
		if (ImGui::Combo("Mode", &current, interpItems, IM_ARRAYSIZE(interpItems)))
		{
			interpolation = static_cast<RB_INTERPOLATION>(current);
		}
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
	void Rigidbody::RegisterHitObject(Object *hitObject)
	{
		hitObjects.insert(hitObject);
	}

	bool Rigidbody::CheckHitObject(Object *hitObject)
	{
		//とうろくされているかされてたらtrue
		return hitObjects.find(hitObject) != hitObjects.end();
	}

	}