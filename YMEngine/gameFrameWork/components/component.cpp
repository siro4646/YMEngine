#include "component.h"

#include "gameFrameWork/gameObject/gameObject.h"
#include "device/device.h"
#include "commandList/commandList.h"
#include "Renderer/renderer.h"

#include "rigidBody/rigidBody.h"
#include "collider/collider.h"

namespace ym
{
	Component::Component(Object *object)
	{
		auto renderer = Renderer::Instance();
		pDevice_ = renderer->GetDevice()->shared_from_this();
		pCmdList_ = renderer->GetGraphicCommandList()->shared_from_this();
		this->object = object;
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