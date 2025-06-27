#include "component.h"
#include "device/device.h"
#include "commandList/commandList.h"
#include "Renderer/renderer.h"

#include "rigidBody/rigidBody.h"
#include "collider/collider.h"

#include <typeinfo>

namespace ym
{
	Component::Component(Object *object)
	{
		auto renderer = Renderer::Instance();
		pDevice_ = renderer->GetDevice()->shared_from_this();
		pCmdList_ = renderer->GetGraphicCommandList()->shared_from_this();
		this->object = object;
	}

	void Component::DrawImgui()
	{
		// �N���X�����擾�i���₷������Ȃ�typeid().name()���琮�`���Ă��ǂ��j
		std::string className = ym::GetShortClassName(*this);

		ImGui::PushID(this);

		if (ImGui::TreeNode(className.c_str()))
		{
			// �g�p�t���O�̐؂�ւ�
			ImGui::Checkbox("Use", &isEnabled);

			// �t���O��ON�̂Ƃ������v���p�e�B�\��
			if (isEnabled)
			{
				DrawImguiBody(); // <- �h���N���X�Ŏ���
			}
			else
			{
				ImGui::TextDisabled("Component is disabled.");
			}

			ImGui::TreePop();
		}
		ImGui::PopID();
	}

	Collision Collision::CreateCollision(Collider *otherCollider, Object *otherObject, Vector3 contactPoint, Vector3 normal)
	{
		Collision a;
		a.collider = otherCollider;
		a.object = otherObject;
		a.rigidbody = otherObject->GetComponent<Rigidbody>().get();
		a.transform = &otherObject->localTransform;
		a.contactPoint = contactPoint;
		a.normal = normal;
		return a;
	}
}
