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
		// クラス名を取得（見やすくするならtypeid().name()から整形しても良い）
		std::string className = ym::GetShortClassName(*this);

		ImGui::PushID(this);

		if (ImGui::TreeNode(className.c_str()))
		{
			// 使用フラグの切り替え
			ImGui::Checkbox("Use", &isEnabled);

			// フラグがONのときだけプロパティ表示
			if (isEnabled)
			{
				DrawImguiBody(); // <- 派生クラスで実装
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
