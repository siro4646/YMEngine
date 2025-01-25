
#include "gameObject.h"
#include "gameFrameWork/components/component.h"
#include "gameFrameWork/gameObject/gameObjectManager.h"


namespace ym
{


	bool Object::AddChild(std::shared_ptr<Object> child)
	{
		if (!child || child.get() == this) return false;

		// 循環参照チェックを追加
		auto ancestor = shared_from_this();
		while (ancestor) {
			if (ancestor == child) {
				return false; // 循環参照が発生するため追加しない
			}
			ancestor = ancestor->_parent;
		}

		if (child->_parent) {
			child->_parent->DeleteChild(child); // 既存の親から削除
		}

		child->SetParent(shared_from_this()); // 新しい親を設定
		_childs.push_back(child);
		return true;
	}


	



	bool Object::DeleteChild(const std::shared_ptr<Object> &child)
	{
		if (!child) return false;

		auto it = std::find(_childs.begin(), _childs.end(), child);
		if (it != _childs.end()) {
			child->SetParent(nullptr); // 親情報をクリア
			_childs.erase(it);
			return true;
		}
		return false;
	}
	void Object::UpdateHierarchy() {
		//親がいるか
		if (_parent != nullptr)
		{
			worldTransform = localTransform.GetWorldTransform(_parent->worldTransform);
		}
		else
		{
			worldTransform = localTransform;
		}
		
	}


	std::shared_ptr<Object> Object::GetParent() const {
		return _parent;
	}

	void Object::SetParent(std::shared_ptr<Object> newParent) {
		if (_parent != newParent) {
			_parent = newParent;
			//NotifyUpdate(); // 親変更時に更新通知
			UpdateHierarchy();
		}
	}


	Object::Object():_parent(nullptr)
	{
	}

	Object::~Object()
	{
		ym::ConsoleLog("[%s]削除 \n", name.c_str());
		//参照を持つだけなので削除しない
		objectManager = nullptr;
		// 自身がデストラクタで削除されるとき、子オブジェクトも削除
		for (auto &child : _childs) {
			child->SetParent(nullptr);
		}
		_childs.clear();
	}

	void Object::FixedUpdate()
	{
		//UpdateHierarchy();
		//子供の固定更新
		for (auto &child : _childs)
		{
			child->FixedUpdate();
		}
		//コンポーネントの固定更新
		for (auto &component : components)
		{
			component->FixedUpdate();
		}
	}

	void Object::Update()
	{
		UpdateHierarchy();
		//子供の更新
		for (auto &child : _childs)
		{
			child->Update();
		}
		//コンポーネントの更新
		for (auto &component : components)
		{
			component->Update();
		}
		//NotifyUpdate();
		//NotifyGlobalUpdate();
		//NotifyGlobalUpdate();

	}

	void Object::Draw()
	{
		//子供の描画
		for (auto &child : _childs)
		{
			child->Draw();
		}
		//コンポーネントの描画
		for (auto &component : components)
		{
			component->Draw();
		}

	}

	void Object::Uninit()
	{
		if (!isUninit)
		{
			isUninit = true;
		}
		else
		{
			return;
		}
		//子供の解放
		for (auto &child : _childs)
		{
			child->Uninit();
		}
		_childs.clear();
		//コンポーネントの解放
		for (auto &component : components)
		{
			component->Uninit();
		}
		components.clear();
	}

} // namespace ym
