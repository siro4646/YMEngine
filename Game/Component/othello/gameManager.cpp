#include "C:\Users\yusuk\Desktop\study\dx12\YMEngine\pch.h"
#include "gameManager.h"

#include "Game/Object/board/gameBoard.h"

#include "Game/Object/koma/koma.h"

#include "Game/Object/board/childBoard.h"

#include "gameFrameWork/gameObject/gameObjectManager.h"

#include "utility/inputSystem/keyBoard/keyBoardInput.h"


namespace ym
{
	void GameManager::Init()
	{
		_childBoards = object->objectManager->FindGameObjects<GameBoard>()[0]->GetChildBoards();
		_currentColor = KomaType::Black;

		// 初期カーソル位置を (3,3) に設定
		x = 3;
		y = 3;

		gameClear = ClearType::None;

		CheckBoard();
	}
	void GameManager::FixedUpdate()
	{
		//CheckBoard();
		if (!isGameClear) {

			isGameClear = true;

			if (gameClear != ClearType::None)
			{
				if (gameClear == ClearType::Player)
				{
					ym::ConsoleLogRelease("Player Win\n");
				}
				else
				{
					ym::ConsoleLogRelease("CPU Win\n");
				}
			}
		}


	}

	void GameManager::Update()
	{
		auto &kb = KeyboardInput::GetInstance();

		static Timer cpuTimer;

				if (kb.GetKeyDown("DOWN") || kb.GetKeyDown("S"))
				{
					y--;
					if (y < 0) y = 0;
					CheckBoard();
				}
				if (kb.GetKeyDown("UP") || kb.GetKeyDown("W"))
				{
					y++;
					if (y > 7) y = 7;
					CheckBoard();
				}
				if (kb.GetKeyDown("RIGHT") || kb.GetKeyDown("D"))
				{
					x++;
					if (x > 7) x = 7;
					CheckBoard();
				}
				if (kb.GetKeyDown("LEFT") || kb.GetKeyDown("A"))
				{
					x--;
					if (x < 0) x = 0;
					CheckBoard();
				}
				if (gameClear == ClearType::None)
				{

					// プレイヤーの操作
					if (_currentColor == KomaType::Black)
					{
						if (!_checkBoard.empty()) // プレイヤーが置ける場所がある場合のみ操作可能
						{

							if (kb.GetKeyDown("SPACE"))
							{
								Put();
							}
							cpuTimer.reset();
						}
						else
						{
							// プレイヤーが置けない場合、CPUのターンへ
							_currentColor = KomaType::White;
							CPUMove();
						}
					}
					else
					{
						// CPUの手番なら自動で手を打つ
						if (cpuTimer.elapsedTime() > 1.0f)
						{
							CPUMove();
							cpuTimer.reset();
						}
					}
				}
	}

	void GameManager::CPUMove()
	{
		int bestMove = GetBestMove(); // AIが最適な手を決める

		if (bestMove != -1) // 有効な手があるなら実行
		{
			int _x = bestMove / 8;
			int _y = bestMove % 8;
			Put(_x,_y); // 駒を置く
		}
		else
		{
			// CPUが動けない場合はターンを交代
			_currentColor = (_currentColor == KomaType::Black) ? KomaType::White : KomaType::Black;
			CheckBoard(); // 再チェック
		}
	}

	int GameManager::GetBestMove()
	{
		std::vector<int> possibleMoves;
		int bestMove = -1;
		int maxFlips = -1;

		// すべてのマスをチェック
		for (int i = 0; i < _childBoards.size(); i++)
		{
			if (CanPlaceKoma(i, _currentColor)) // 置けるか判定
			{
				int flipCount = CountFlippableDisks(i, _currentColor); // ひっくり返せる数を計算
				possibleMoves.push_back(i);
				// 一番多くひっくり返せる場所を選択
				if (flipCount > maxFlips)
				{
					maxFlips = flipCount;
					bestMove = i;
				}
			}
		}

		if (possibleMoves.empty())
		{
			return -1; // どこにも置けない場合
		}

		// ランダムAIにするなら以下のコメントを外す
		// return possibleMoves[rand() % possibleMoves.size()];
		return bestMove; // 最も多くひっくり返せる場所を返す
	}

	int GameManager::CountFlippableDisks(int index, KomaType currentColor)
	{
		const int boardSize = 8;
		int x = index % boardSize;
		int y = index / boardSize;

		static const int dx[8] = { -1, 1,  0,  0, -1, 1, -1, 1 };
		static const int dy[8] = { 0, 0, -1,  1, -1, 1,  1, -1 };

		int flipCount = 0;

		for (int d = 0; d < 8; d++)
		{
			int nx = x + dx[d];
			int ny = y + dy[d];
			int nIndex = ny * boardSize + nx;
			bool foundOpponent = false;
			int count = 0;

			while (nx >= 0 && nx < boardSize && ny >= 0 && ny < boardSize)
			{
				nIndex = ny * boardSize + nx;

				if (_childBoards[nIndex]->GetKomaType() == KomaType::None)
				{
					break;
				}

				if (_childBoards[nIndex]->GetKomaType() != currentColor)
				{
					foundOpponent = true;
					count++;
				}
				else
				{
					if (foundOpponent)
					{
						flipCount += count;
					}
					break;
				}

				nx += dx[d];
				ny += dy[d];
			}
		}

		return flipCount;
	}



	void GameManager::Draw()
	{
	}

	void GameManager::Uninit()
	{
	}

	void GameManager::Put()
	{
		int index = x * 8 + y;

		for (auto &board : _checkBoard)
		{
			if (board == _childBoards[index]) // 置ける場所にある場合のみ駒を置く
			{
				_childBoards[index]->PutKoma(_currentColor); // 駒を置く
				FlipKoma(index, _currentColor); // ひっくり返す処理を実行

				// ターン交代
				_currentColor = (_currentColor == KomaType::Black) ? KomaType::White : KomaType::Black;

				CheckBoard(); // 盤面の更新
				break;
			}
		}
	}
	void GameManager::Put(int _x, int _y)
	{
		int index = _x * 8 + _y;

		for (auto &board : _checkBoard)
		{
			if (board == _childBoards[index]) // 置ける場所にある場合のみ駒を置く
			{
				_childBoards[index]->PutKoma(_currentColor); // 駒を置く
				FlipKoma(index, _currentColor); // ひっくり返す処理を実行

				// ターン交代
				_currentColor = (_currentColor == KomaType::Black) ? KomaType::White : KomaType::Black;

				CheckBoard(); // 盤面の更新
				break;
			}
		}

	}
	void GameManager::FlipKoma(int index, KomaType currentColor)
	{
		const int boardSize = 8;
		int x = index % boardSize;
		int y = index / boardSize;

		// 8方向（上下左右 + 斜め）
		static const int dx[8] = { -1, 1,  0,  0, -1, 1, -1, 1 };
		static const int dy[8] = { 0, 0, -1,  1, -1, 1,  1, -1 };

		// ひっくり返す処理
		for (int d = 0; d < 8; d++)
		{
			std::vector<int> flipIndexes; // ひっくり返す駒のリスト

			int nx = x + dx[d];
			int ny = y + dy[d];
			int nIndex = ny * boardSize + nx;

			bool foundOpponent = false;

			while (nx >= 0 && nx < boardSize && ny >= 0 && ny < boardSize)
			{
				nIndex = ny * boardSize + nx;

				// 空白なら終了（ひっくり返せない）
				if (_childBoards[nIndex]->GetKomaType() == KomaType::None)
				{
					break;
				}

				// 相手の駒ならリストに追加
				if (_childBoards[nIndex]->GetKomaType() != currentColor)
				{
					foundOpponent = true;
					flipIndexes.push_back(nIndex);
				}
				// 自分の駒があったら、間に相手の駒がある場合のみ反転
				else
				{
					if (foundOpponent)
					{
						int count = 0;
						for (int flipIndex : flipIndexes)
						{
							if (currentColor == KomaType::Black)
							{
								//_childBoards[flipIndex]->_childs[0]->localTransform.Rotation.x = 180;
								//_childBoards[flipIndex]->SetTexture("black")
							}
							else
							{
								//_childBoards[flipIndex]->_childs[0]->localTransform.Rotation.x = 0;
								//_childBoards[flipIndex]->SetTexture("white");

							}
							//ym::ConsoleLogRelease("%d\n",flipIndex);
							_childBoards[flipIndex]->FlipKoma(currentColor,count * 0.2);
							ym::ConsoleLogRelease("%f\n", count*0.2f);
							count++;
						}
					}
					break;
				}

				// 次のマスへ移動
				nx += dx[d];
				ny += dy[d];
			}
		}
	}



	void GameManager::CheckBoard()
	{
		_checkBoard.clear();
		bool canPlacePlayer = false;
		bool canPlaceCPU = false;

		for (int i = 0; i < _childBoards.size(); i++)
		{
			if (_childBoards[i]->GetKomaType() == KomaType::None) // 何も置かれていない場合のみ判定
			{
				if (CanPlaceKoma(i, _currentColor)) // 置けるか判定
				{
					_childBoards[i]->SetTexture("lightGreen");
					_checkBoard.push_back(_childBoards[i]);

					

					if (_currentColor == KomaType::Black)
					{
						canPlacePlayer = true;
					}
					else
					{
						canPlaceCPU = true;
					}
				}
				else
				{
					_childBoards[i]->SetTexture("green");
				}
			}
			else
			{
				_childBoards[i]->SetTexture("green");
			}
		}

		// 今カーソルがある位置を光らせる
		_childBoards[x * 8 + y]->SetTexture("cyen");

		// **どちらのプレイヤーも置けない場合、ゲーム終了**
		if (!canPlacePlayer && !canPlaceCPU)
		{
			
				GameOver();
			
		}
	}

	void GameManager::GameOver()
	{
		int blackCount = 0;
		int whiteCount = 0;

		for (auto &board : _childBoards)
		{
			if (board->GetKomaType() == KomaType::Black) blackCount++;
			if (board->GetKomaType() == KomaType::White) whiteCount++;
		}

		if (blackCount < whiteCount)
		{
			gameClear = ClearType::Player;
		}
		else if (whiteCount < blackCount)
		{
			gameClear = ClearType::CPU;
		}
		else
		{
			gameClear = ClearType::None;
		}
		//ym::ConsoleLogRelease(result.c_str());
	}

	bool GameManager::CanPlaceKoma(int index, KomaType currentColor)
	{
		// 盤のサイズ
		const int boardSize = 8;
		const int x = index % boardSize;
		const int y = index / boardSize;

		// 駒の色がNone以外なら置けない
		if (_childBoards[index]->GetKomaType() != KomaType::None)
		{
			return false;
		}

		// 8方向（上下左右 + 斜め）
		static const int dx[8] = { -1, 1,  0,  0, -1, 1, -1, 1 };
		static const int dy[8] = { 0, 0, -1,  1, -1, 1,  1, -1 };

		// 各方向をチェック
		for (int d = 0; d < 8; d++)
		{
			int nx = x + dx[d];
			int ny = y + dy[d];
			int nIndex = ny * boardSize + nx;

			bool foundOpponent = false;

			// 盤外チェック
			while (nx >= 0 && nx < boardSize && ny >= 0 && ny < boardSize)
			{
				// 盤面のインデックス計算
				nIndex = ny * boardSize + nx;

				// 空白なら終了
				if (_childBoards[nIndex]->GetKomaType() == KomaType::None)
				{
					break;
				}

				// 相手の駒ならフラグを立てて継続
				if (_childBoards[nIndex]->GetKomaType() != currentColor)
				{
					foundOpponent = true;
				}
				// 自分の駒があったら、間に相手の駒がある場合のみ置ける
				else
				{
					if (foundOpponent)
					{
						return true;
					}
					break;
				}

				// 次のマスへ移動
				nx += dx[d];
				ny += dy[d];
			}
		}

		return false;
	}



}
