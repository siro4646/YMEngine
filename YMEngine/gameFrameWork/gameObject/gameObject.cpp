
#include "gameObject.h"
#include "gameFrameWork/components/component.h"
#include "gameFrameWork/gameObject/gameObjectManager.h"


namespace ym
{


	bool Object::AddChild(std::shared_ptr<Object> child)
	{
		if (!child || child.get() == this) return false;

		// �z�Q�ƃ`�F�b�N��ǉ�
		auto ancestor = shared_from_this();
		while (ancestor) {
			if (ancestor == child) {
				return false; // �z�Q�Ƃ��������邽�ߒǉ����Ȃ�
			}
			ancestor = ancestor->_parent;
		}

		if (child->_parent) {
			child->_parent->DeleteChild(child); // �����̐e����폜
		}

		child->SetParent(shared_from_this()); // �V�����e��ݒ�
		_childs.push_back(child);
		return true;
	}


	



	bool Object::DeleteChild(const std::shared_ptr<Object> &child)
	{
		if (!child) return false;

		auto it = std::find(_childs.begin(), _childs.end(), child);
		if (it != _childs.end()) {
			child->SetParent(nullptr); // �e�����N���A
			_childs.erase(it);
			return true;
		}
		return false;
	}
	void Object::UpdateHierarchy() {
		//�e�����邩
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
			//NotifyUpdate(); // �e�ύX���ɍX�V�ʒm
			UpdateHierarchy();
		}
	}


	Object::Object():_parent(nullptr)
	{
	}

	Object::~Object()
	{
		ym::ConsoleLog("[%s]�폜 \n", name.c_str());
		//�Q�Ƃ��������Ȃ̂ō폜���Ȃ�
		objectManager = nullptr;
		// ���g���f�X�g���N�^�ō폜�����Ƃ��A�q�I�u�W�F�N�g���폜
		for (auto &child : _childs) {
			child->SetParent(nullptr);
		}
		_childs.clear();
	}

	void Object::FixedUpdate()
	{
		//UpdateHierarchy();
		//�q���̌Œ�X�V
		for (auto &child : _childs)
		{
			child->FixedUpdate();
		}
		//�R���|�[�l���g�̌Œ�X�V
		for (auto &component : components)
		{
			component->FixedUpdate();
		}
	}

	void Object::Update()
	{
		UpdateHierarchy();
		//�q���̍X�V
		for (auto &child : _childs)
		{
			child->Update();
		}
		//�R���|�[�l���g�̍X�V
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
		//�q���̕`��
		for (auto &child : _childs)
		{
			child->Draw();
		}
		//�R���|�[�l���g�̕`��
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
		//�q���̉��
		for (auto &child : _childs)
		{
			child->Uninit();
		}
		_childs.clear();
		//�R���|�[�l���g�̉��
		for (auto &component : components)
		{
			component->Uninit();
		}
		components.clear();
	}

} // namespace ym
