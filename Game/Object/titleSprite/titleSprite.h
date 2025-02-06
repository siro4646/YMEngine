#pragma once

#include "gameFrameWork/gameObject/gameObject.h"
namespace ym {



	//コピー用テンプレート
	class TitleSprite : public Object
	{
	public:
		void Init()override;
		void FixedUpdate()override;
		void Update()override;
		void Draw()override;
		void Uninit()override;
		std::shared_ptr<Object>Clone()override;

		

	private:	
		std::shared_ptr<Object>camera = nullptr;
		bool isGameStart = false;
	};
}