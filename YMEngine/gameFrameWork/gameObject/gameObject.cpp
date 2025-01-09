
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

	void Object::NotifyUpdate()
	{
		//���[�J���A�e�̕ύX����O���[�o�����W�̍X�V
		if (_parent != nullptr)
		{
			//���W
			const auto m = CreateTransformMatrix(Transform(
				_parent->globalTransform.Position,
				_parent->globalTransform.Scale,
				_parent->globalTransform.Rotation
			));
			std::array<float, 3> p = { localTransform.Position.x,localTransform.Position.y,localTransform.Position.z };
			globalTransform.Position = TransformPoint(m, p);
			globalTransform.Position = _parent->globalTransform.Position + localTransform.Position;
			globalTransform.Rotation = _parent->globalTransform.Rotation + localTransform.Rotation;
			globalTransform.Scale = _parent->globalTransform.Scale * localTransform.Scale;
		}
		else
		{
			globalTransform = localTransform;
		}

		//�q���̍X�V
		for (auto &child : _childs)
		{
			if (child != nullptr)
				child->NotifyUpdate();
		}
		NotifyGlobalUpdate();
	}

	void Object::NotifyGlobalUpdate()
	{
		//�O���[�o�����W��ύX�����Ƃ��Ƀ��[�J��������
		if (_parent != nullptr)
		{
			const auto inverse = CreateTransformMatrix(Transform(
				Vector3(-_parent->globalTransform.Position.x,
					-_parent->globalTransform.Position.y,
					-_parent->globalTransform.Position.z
				),
				1.f / _parent->globalTransform.Scale,
				Vector3(-_parent->globalTransform.Rotation.x,
					-_parent->globalTransform.Rotation.y,
					-_parent->globalTransform.Rotation.z
				)
			));
			std::array<float, 3> relative = {
				globalTransform.Position.x - _parent->globalTransform.Position.x,
				globalTransform.Position.y - _parent->globalTransform.Position.y,
				globalTransform.Position.z - _parent->globalTransform.Position.z
			};
			localTransform.Position = TransformPoint(inverse, relative);
			localTransform.Rotation = globalTransform.Rotation - _parent->globalTransform.Rotation;
			localTransform.Scale = globalTransform.Scale / _parent->globalTransform.Scale;
		}
	}
	std::shared_ptr<Object> Object::GetParent() const {
		return _parent;
	}

	void Object::SetParent(std::shared_ptr<Object> newParent) {
		if (_parent != newParent) {
			_parent = newParent;
			NotifyUpdate(); // �e�ύX���ɍX�V�ʒm
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
		//�q���̉��
		for (auto &child : _childs)
		{
			child->Uninit();
		}
		//�R���|�[�l���g�̉��
		for (auto &component : components)
		{
			component->Uninit();
		}
	}

} // namespace ym
