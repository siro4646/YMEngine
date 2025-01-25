#pragma once

//#include "utility/transform/transform.h"

namespace ym
{

	class GameObjectManager;
	class Component;



	//コピー用テンプレート
	//class ClassName : public Object
	//{
	//public:
	//	void Init()override;
	//	void FixedUpdate()override;
	//	void Update()override;
	//	void Draw()override;
	//	void Uninit()override;
	//  std::shared_ptr<Object>Clone()override;
	//};


	class Object:public std::enable_shared_from_this<Object>
	{
	public:
		enum Type
		{
			Delete,
			None,
			Max,
		};

		GameObjectManager *objectManager;//参照だけもつ
		Transform localTransform;
		Transform worldTransform;
		Type type =Type::None;
		std::vector<std::shared_ptr<Component>> components;
		std::string name;
	private:
		bool isUninit = false;
	protected:
		std::string tag;
		std::shared_ptr<Object>_parent = nullptr;
		std::vector <std::shared_ptr<Object>> _childs;
	public:

		Object();
		virtual ~Object();

		//初期化
		virtual void Init() = 0;

		//固定更新
		virtual void FixedUpdate();

		//更新
		virtual void Update();
			
		//描画
		virtual void Draw();
		//解放
		virtual void Uninit();

		// 親オブジェクトを取得
		std::shared_ptr<Object> GetParent() const; 

		// 親オブジェクトを設定
		void SetParent(std::shared_ptr<Object> newParent);


		virtual std::shared_ptr<Object>Clone() = 0;
		//子供の追加(動くか未検証)
		bool AddChild(std::shared_ptr<Object> child);
		//子供の削除
		bool DeleteChild(const std::shared_ptr<Object> &child);

		void UpdateHierarchy();


	

		//コンポーネントの追加
		template <typename T> std::shared_ptr<T> AddComponent()
		{
			// スマートポインタでメモリ管理
			auto component = std::make_shared<T>(this);
			component->Init();
			components.push_back(component);
			return component;
		}
		//コンポーネントの取得
		template <typename T> std::shared_ptr<T> GetComponent()
		{
			for (auto &component : components)
			{
				// dynamic_cast を使用して、T 型にキャスト可能か判定
				if (auto castedComponent = std::dynamic_pointer_cast<T>(component))
				{
					return castedComponent;
				}
			}
			return nullptr;
		}
	};

}


