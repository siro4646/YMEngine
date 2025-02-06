#pragma once

#include "gameFrameWork/gameObject/gameObject.h"
namespace ym {

	class ChildBoard;

	//コピー用テンプレート
	class GameBoard : public Object
	{
	public:
		void Init()override;
		void FixedUpdate()override;
		void Update()override;
		void Draw()override;
		void Uninit()override;
		std::shared_ptr<Object>Clone()override;

		vector<ChildBoard *> GetChildBoards() { return _childBoards; }

		//Object

	private:
		std::vector<ChildBoard *> _childBoards;
	};
}