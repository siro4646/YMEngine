#pragma once
#include "gameFrameWork/components/component.h"
#include "gameObject/gameObject.h"

#include "gameFrameWork/raycast/raycast.h"

#include "gameFrameWork/camera/cameraManager.h"
#include "gameFrameWork/camera/camera.h"

#include "components/mesh/renderer/meshRenderer.h"

namespace ym
{
	class TestComponent : public Component
	{
	public:
		using Component::Component;

		void Init()override {}
		void FixedUpdate() override {}
		void Update()override {
			if (MouseInput::Instance().GetKeyDown("left"))
			{
				Ray ray = CameraManager::Instance().GetMainCamera()->ScreenPointToRay();
				//ym::ConsoleLogRelease("Ray Origin: (%f, %f, %f)\n", ray.origin.x, ray.origin.y, ray.origin.z);
				//ym::ConsoleLogRelease("Ray Direction: (%f, %f, %f)\n", ray.direction.x, ray.direction.y, ray.direction.z);
				RaycastHit hit;
				std::vector<std::shared_ptr<Object>> objects;
				// �V�[�����̑S�ẴI�u�W�F�N�g���擾
				objects = object->objectManager->GetGameObjects();

				if (Raycast(ray, 100.0f, hit, objects))
				{
					DebugLog("Hit detected at position: (%f, %f, %f)\n", hit.point.x, hit.point.y, hit.point.z);
					auto mr = hit.hitObject->GetComponent<mesh::MeshRenderer>().get();
					if (mr)
					{
						DebugLog("Hit detected on object: %s\n", hit.hitObject->name.c_str());
						auto mat = mr->GetMaterials();
						Vector3 col;
						col.x = ym::GlobalRandom.GetFValueRange(0.0f, 1.0f); // �����_���ȐF�𐶐�
						col.y = ym::GlobalRandom.GetFValueRange(0.0f, 1.0f); // �����_���ȐF�𐶐�
						col.z = ym::GlobalRandom.GetFValueRange(0.0f, 1.0f); // �����_���ȐF�𐶐�
						mat[0]->SetFloat3("testValue",col.ToXMFLOAT3()); // ��: �}�e���A���̃v���p�e�B���X�V

					}

					//hit.hitObject->type = Object::Delete; // ��: �Փ˂����I�u�W�F�N�g�̃^�C�v��ݒ�
				}
				else
				{
					//ym::ConsoleLogRelease("No hit detected.");
				}

			}
		}
		void Draw()override {}
		void DrawImguiBody() override {
			ImGui::Text("Test Component");
			ImGui::SliderFloat("Y Position", &yPosition, -10.0f, 10.0f, "%.1f");
		}
		void Uninit()override {}
		const char *GetName() const override { return "TestComponent"; }

		void OnCollisionEnter(Collision &col) override
		{
			// �Փ˂����������Ƃ��̏���
			std::cout << "Collision Entered with: " << col.object->name << std::endl;

			auto pos = object->localTransform.Position;
			object->localTransform.Position = Vector3(pos.x, yPosition, pos.z); // ��: Y���W��2�ɐݒ肵�ăI�u�W�F�N�g����Ɉړ�
		}
	private:
		float yPosition = 2.0f; // Y���W�̏����l
	};
	REGISTER_COMPONENT(TestComponent);
}