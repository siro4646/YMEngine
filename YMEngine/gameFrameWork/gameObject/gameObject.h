#pragma once

//#include "utility/transform/transform.h"

namespace ym
{

	class ObjectManager;
	class Component;

	//�R�s�[�p�e���v���[�g
	//class ClassName : public Object
	//{
	//public:
	//	void Init()override;
	//	void FixedUpdate()override;
	//	void Update()override;
	//	void Draw()override;
	//	void Uninit()override;
	//  Object*Clone()override;
	//};


	class Object:public std::enable_shared_from_this<Object>
	{
	public:
		ObjectManager *objectManager;
		Transform localTransform;
		Transform globalTransform;

		std::vector<std::shared_ptr<Component>> components;
	private:
		std::string tag;
		std::string name;
	protected:
		std::shared_ptr<Object>_parent = nullptr;
		std::vector <std::shared_ptr<Object>> _childs;
	public:

		Object();
		virtual ~Object();

		//������
		virtual void Init() = 0;

		//�Œ�X�V
		virtual void FixedUpdate();

		//�X�V
		virtual void Update();
			
		//�`��
		virtual void Draw();
		//���
		virtual void Uninit();

		// �e�I�u�W�F�N�g���擾
		std::shared_ptr<Object> GetParent() const; 

		// �e�I�u�W�F�N�g��ݒ�
		void SetParent(std::shared_ptr<Object> newParent);


		virtual std::shared_ptr<Object>Clone() = 0;
		//�q���̒ǉ�(������������)
		bool AddChild(std::shared_ptr<Object> child);
		//�q���̍폜
		bool DeleteChild(const std::shared_ptr<Object> &child);

		//�e�q�֌W�̍X�V
		void NotifyUpdate();
		//�e�q�֌W�̍X�V
		void NotifyGlobalUpdate();

	

		//�R���|�[�l���g�̒ǉ�
		template <typename T> std::shared_ptr<T> AddComponent()
		{
			// �X�}�[�g�|�C���^�Ń������Ǘ�
			auto component = std::make_shared<T>(this);
			component->Init();
			components.push_back(component);
			return component;
		}
		//�R���|�[�l���g�̎擾
		template <typename T> std::shared_ptr<T> GetComponent()
		{
			for (auto &component : components)
			{
				if (typeid(*component) == typeid(T))
				{
					return std::dynamic_pointer_cast<T>(component);
				}
			}
			return nullptr;
		}
	};

}


