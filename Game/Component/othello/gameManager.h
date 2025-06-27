#pragma once

#include "gameFrameWork/components/component.h"

namespace ym
{
	enum class ClearType
	{
		None,
		Player,
		CPU,
	};

	class ChildBoard;
	enum class KomaType;
	class GameManager : public Component
	{
	public:
		using Component::Component;

		void Init()override;
		void FixedUpdate() override;
		void Update()override;
		void CPUMove();
		int GetBestMove();
		int CountFlippableDisks(int index, KomaType currentColor);
		void Draw()override;
		void Uninit()override;
		const char *GetName() const override { return "GameManager"; }

		void Put();
		void Put(int _x, int _y);

		void FlipKoma(int index, KomaType currentColor);


	private:

		KomaType _currentColor{};

		int x = 0;
		int y = 0;

		void CheckBoard();
		void GameOver();
		bool CanPlaceKoma(int index, KomaType currentColor);
		vector<ChildBoard *> _childBoards;
		std::vector<ChildBoard *> _checkBoard;

		ClearType gameClear = {};
		bool isGameClear;

	};

}