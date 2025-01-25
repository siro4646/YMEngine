#include "warpGateManager.h"
#include "Game/Object/warpGate/warpGate.h"
#include "gameFrameWork/gameObject/gameObjectManager.h"
namespace ym
{
	void WarpGateManager::Init()
	{
		name = "WarpGateManager";

		warpGates1 = std::make_shared<WarpGate>();
		objectManager->AddGameObject(warpGates1);

		warpGates2 = std::make_shared<WarpGate>();
		objectManager->AddGameObject(warpGates2);

		warpGates1->localTransform.Position.y = 2.0f;
		warpGates1->localTransform.Position.z = 2.0f;
		warpGates1->localTransform.Position.x = -4.0f;
		warpGates1->SetCameraName("arpCamera");
		warpGates2->localTransform.Position.z = -2.0f;
		warpGates2->localTransform.Position.x = 2.0f;
		warpGates2->localTransform.Rotation.y = 90.0f;
		warpGates2->SetCameraName("warpCamera");
	}

	void WarpGateManager::FixedUpdate()
	{
	}

	void WarpGateManager::Update()
	{
	}

	void WarpGateManager::Draw()
	{
	}

	void WarpGateManager::Uninit()
	{
	}

	std::shared_ptr<Object> WarpGateManager::Clone()
	{
		auto copy = std::make_shared<WarpGateManager>(*this); // コピーコンストラクタを使用
		// 子オブジェクトは深いコピーを行う
		for (const auto &child : _childs) {
			copy->_childs.push_back(child->Clone());
		}
		return copy;
	}
}
