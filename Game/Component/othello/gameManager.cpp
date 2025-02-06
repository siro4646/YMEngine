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

		// �����J�[�\���ʒu�� (3,3) �ɐݒ�
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

					// �v���C���[�̑���
					if (_currentColor == KomaType::Black)
					{
						if (!_checkBoard.empty()) // �v���C���[���u����ꏊ������ꍇ�̂ݑ���\
						{

							if (kb.GetKeyDown("SPACE"))
							{
								Put();
							}
							cpuTimer.reset();
						}
						else
						{
							// �v���C���[���u���Ȃ��ꍇ�ACPU�̃^�[����
							_currentColor = KomaType::White;
							CPUMove();
						}
					}
					else
					{
						// CPU�̎�ԂȂ玩���Ŏ��ł�
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
		int bestMove = GetBestMove(); // AI���œK�Ȏ�����߂�

		if (bestMove != -1) // �L���Ȏ肪����Ȃ���s
		{
			int _x = bestMove / 8;
			int _y = bestMove % 8;
			Put(_x,_y); // ���u��
		}
		else
		{
			// CPU�������Ȃ��ꍇ�̓^�[�������
			_currentColor = (_currentColor == KomaType::Black) ? KomaType::White : KomaType::Black;
			CheckBoard(); // �ă`�F�b�N
		}
	}

	int GameManager::GetBestMove()
	{
		std::vector<int> possibleMoves;
		int bestMove = -1;
		int maxFlips = -1;

		// ���ׂẴ}�X���`�F�b�N
		for (int i = 0; i < _childBoards.size(); i++)
		{
			if (CanPlaceKoma(i, _currentColor)) // �u���邩����
			{
				int flipCount = CountFlippableDisks(i, _currentColor); // �Ђ�����Ԃ��鐔���v�Z
				possibleMoves.push_back(i);
				// ��ԑ����Ђ�����Ԃ���ꏊ��I��
				if (flipCount > maxFlips)
				{
					maxFlips = flipCount;
					bestMove = i;
				}
			}
		}

		if (possibleMoves.empty())
		{
			return -1; // �ǂ��ɂ��u���Ȃ��ꍇ
		}

		// �����_��AI�ɂ���Ȃ�ȉ��̃R�����g���O��
		// return possibleMoves[rand() % possibleMoves.size()];
		return bestMove; // �ł������Ђ�����Ԃ���ꏊ��Ԃ�
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
			if (board == _childBoards[index]) // �u����ꏊ�ɂ���ꍇ�̂݋��u��
			{
				_childBoards[index]->PutKoma(_currentColor); // ���u��
				FlipKoma(index, _currentColor); // �Ђ�����Ԃ����������s

				// �^�[�����
				_currentColor = (_currentColor == KomaType::Black) ? KomaType::White : KomaType::Black;

				CheckBoard(); // �Ֆʂ̍X�V
				break;
			}
		}
	}
	void GameManager::Put(int _x, int _y)
	{
		int index = _x * 8 + _y;

		for (auto &board : _checkBoard)
		{
			if (board == _childBoards[index]) // �u����ꏊ�ɂ���ꍇ�̂݋��u��
			{
				_childBoards[index]->PutKoma(_currentColor); // ���u��
				FlipKoma(index, _currentColor); // �Ђ�����Ԃ����������s

				// �^�[�����
				_currentColor = (_currentColor == KomaType::Black) ? KomaType::White : KomaType::Black;

				CheckBoard(); // �Ֆʂ̍X�V
				break;
			}
		}

	}
	void GameManager::FlipKoma(int index, KomaType currentColor)
	{
		const int boardSize = 8;
		int x = index % boardSize;
		int y = index / boardSize;

		// 8�����i�㉺���E + �΂߁j
		static const int dx[8] = { -1, 1,  0,  0, -1, 1, -1, 1 };
		static const int dy[8] = { 0, 0, -1,  1, -1, 1,  1, -1 };

		// �Ђ�����Ԃ�����
		for (int d = 0; d < 8; d++)
		{
			std::vector<int> flipIndexes; // �Ђ�����Ԃ���̃��X�g

			int nx = x + dx[d];
			int ny = y + dy[d];
			int nIndex = ny * boardSize + nx;

			bool foundOpponent = false;

			while (nx >= 0 && nx < boardSize && ny >= 0 && ny < boardSize)
			{
				nIndex = ny * boardSize + nx;

				// �󔒂Ȃ�I���i�Ђ�����Ԃ��Ȃ��j
				if (_childBoards[nIndex]->GetKomaType() == KomaType::None)
				{
					break;
				}

				// ����̋�Ȃ烊�X�g�ɒǉ�
				if (_childBoards[nIndex]->GetKomaType() != currentColor)
				{
					foundOpponent = true;
					flipIndexes.push_back(nIndex);
				}
				// �����̋��������A�Ԃɑ���̋����ꍇ�̂ݔ��]
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

				// ���̃}�X�ֈړ�
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
			if (_childBoards[i]->GetKomaType() == KomaType::None) // �����u����Ă��Ȃ��ꍇ�̂ݔ���
			{
				if (CanPlaceKoma(i, _currentColor)) // �u���邩����
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

		// ���J�[�\��������ʒu�����点��
		_childBoards[x * 8 + y]->SetTexture("cyen");

		// **�ǂ���̃v���C���[���u���Ȃ��ꍇ�A�Q�[���I��**
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
		// �Ղ̃T�C�Y
		const int boardSize = 8;
		const int x = index % boardSize;
		const int y = index / boardSize;

		// ��̐F��None�ȊO�Ȃ�u���Ȃ�
		if (_childBoards[index]->GetKomaType() != KomaType::None)
		{
			return false;
		}

		// 8�����i�㉺���E + �΂߁j
		static const int dx[8] = { -1, 1,  0,  0, -1, 1, -1, 1 };
		static const int dy[8] = { 0, 0, -1,  1, -1, 1,  1, -1 };

		// �e�������`�F�b�N
		for (int d = 0; d < 8; d++)
		{
			int nx = x + dx[d];
			int ny = y + dy[d];
			int nIndex = ny * boardSize + nx;

			bool foundOpponent = false;

			// �ՊO�`�F�b�N
			while (nx >= 0 && nx < boardSize && ny >= 0 && ny < boardSize)
			{
				// �Ֆʂ̃C���f�b�N�X�v�Z
				nIndex = ny * boardSize + nx;

				// �󔒂Ȃ�I��
				if (_childBoards[nIndex]->GetKomaType() == KomaType::None)
				{
					break;
				}

				// ����̋�Ȃ�t���O�𗧂ĂČp��
				if (_childBoards[nIndex]->GetKomaType() != currentColor)
				{
					foundOpponent = true;
				}
				// �����̋��������A�Ԃɑ���̋����ꍇ�̂ݒu����
				else
				{
					if (foundOpponent)
					{
						return true;
					}
					break;
				}

				// ���̃}�X�ֈړ�
				nx += dx[d];
				ny += dy[d];
			}
		}

		return false;
	}



}
