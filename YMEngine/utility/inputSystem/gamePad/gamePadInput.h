#pragma once
#include "../base/inputDevice.h"
#include <unordered_map>

//ゲームパッド用
#include <Xinput.h>
#pragma comment (lib, "Xinput.lib")

namespace ym
{




	class GamePadInput : public InputDevice {
	private:
		XINPUT_STATE state;
		XINPUT_VIBRATION vibration;
		bool up[16];
		bool down[16];

		//stringからキー情報に変換用マップ
		std::unordered_map<std::string, UINT> keyMap;

		GamePadInput()
		{
			//コントローラーの初期化
			{
				for (int i = 0; i < 16; i++)
				{
					up[i] = true;
					down[i] = false;
				}
				vibration.wLeftMotorSpeed = 0;
				vibration.wRightMotorSpeed = 0;
			}
		}
		~GamePadInput()
		{
			vibration.wLeftMotorSpeed = 0;
			vibration.wRightMotorSpeed = 0;
			XInputSetState(0, &vibration);
		}

		HRESULT CreateKeyMap()
		{
			//大文字
			keyMap["A"] = XINPUT_GAMEPAD_A;
			keyMap["B"] = XINPUT_GAMEPAD_B;
			keyMap["X"] = XINPUT_GAMEPAD_X;
			keyMap["Y"] = XINPUT_GAMEPAD_Y;
			keyMap["UP"] = XINPUT_GAMEPAD_DPAD_UP;
			keyMap["DOWN"] = XINPUT_GAMEPAD_DPAD_DOWN;
			keyMap["LEFT"] = XINPUT_GAMEPAD_DPAD_LEFT;
			keyMap["RIGHT"] = XINPUT_GAMEPAD_DPAD_RIGHT;
			keyMap["L"] = XINPUT_GAMEPAD_LEFT_SHOULDER;
			keyMap["R"] = XINPUT_GAMEPAD_RIGHT_SHOULDER;
			keyMap["START"] = XINPUT_GAMEPAD_START;
			keyMap["BACK"] = XINPUT_GAMEPAD_BACK;
			keyMap["L3"] = XINPUT_GAMEPAD_LEFT_THUMB;
			keyMap["R3"] = XINPUT_GAMEPAD_RIGHT_THUMB;
			//小文字
			keyMap["a"] = XINPUT_GAMEPAD_A;
			keyMap["b"] = XINPUT_GAMEPAD_B;
			keyMap["x"] = XINPUT_GAMEPAD_X;
			keyMap["y"] = XINPUT_GAMEPAD_Y;
			keyMap["up"] = XINPUT_GAMEPAD_DPAD_UP;
			keyMap["down"] = XINPUT_GAMEPAD_DPAD_DOWN;
			keyMap["left"] = XINPUT_GAMEPAD_DPAD_LEFT;
			keyMap["right"] = XINPUT_GAMEPAD_DPAD_RIGHT;
			keyMap["l"] = XINPUT_GAMEPAD_LEFT_SHOULDER;
			keyMap["r"] = XINPUT_GAMEPAD_RIGHT_SHOULDER;
			keyMap["start"] = XINPUT_GAMEPAD_START;
			keyMap["back"] = XINPUT_GAMEPAD_BACK;
			keyMap["l3"] = XINPUT_GAMEPAD_LEFT_THUMB;
			keyMap["r3"] = XINPUT_GAMEPAD_RIGHT_THUMB;
			//先頭だけ大文字
			keyMap["Left"] = XINPUT_GAMEPAD_DPAD_LEFT;
			keyMap["Right"] = XINPUT_GAMEPAD_DPAD_RIGHT;
			keyMap["Up"] = XINPUT_GAMEPAD_DPAD_UP;
			keyMap["Down"] = XINPUT_GAMEPAD_DPAD_DOWN;
			keyMap["L"] = XINPUT_GAMEPAD_LEFT_SHOULDER;
			keyMap["R"] = XINPUT_GAMEPAD_RIGHT_SHOULDER;
			keyMap["Start"] = XINPUT_GAMEPAD_START;
			keyMap["Back"] = XINPUT_GAMEPAD_BACK;


			return S_OK;
		}


	public:
		bool GetKey(const std::string &key) override;
		bool GetKeyDown(const std::string &key) override;
		bool GetKeyUp(const std::string &key) override;

		static GamePadInput &GetInstance();

		void Initialize();


	};

}