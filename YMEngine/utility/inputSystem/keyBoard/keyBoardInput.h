#pragma once
#include "../base/inputDevice.h"
#include <unordered_map>

//キーボード用
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#pragma comment (lib, "dinput8.lib")
#pragma comment(lib, "DXGUID.LIB")
// キー最大数
#define KEY_MAX 256
namespace ym {

	class KeyboardInput : public InputDevice {
	private:

		// ウィンドウ
		static HWND *win;
		// 参照結果
		static HRESULT result;
		// インプット
		static LPDIRECTINPUT8 input;
		// インプットデバイス
		static LPDIRECTINPUTDEVICE8 key;
		// キー情報
		static BYTE keys[KEY_MAX];
		// 前のキー情報
		static BYTE olds[KEY_MAX];
		// stringからキー情報に変換用マップ
		static std::unordered_map<std::string, UINT> keyMap;


		// インプットの生成
		HRESULT CreateInput(void)
		{
			result = DirectInput8Create(GetModuleHandle(0), DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)(&input),
				NULL);

			return result;
		}

		// キーデバイスの生成
		HRESULT CreateKey(void)
		{
			result = input->CreateDevice(GUID_SysKeyboard, &key, NULL);

			return result;
		}


		// キーフォーマットのセット
		HRESULT SetKeyFormat(void)
		{
			result = key->SetDataFormat(&c_dfDIKeyboard);

			return result;
		}
		// キーの協調レベルのセット
		HRESULT SetKeyCooperative(void)
		{
			result = key->SetCooperativeLevel(*win, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

			//入力デバイスへのアクセス権利を取得
			key->Acquire();

			return result;
		}

		//stringからキー情報に変換するマップの作成
		HRESULT CreateKeyMap(void)
		{
			keyMap["A"] = DIK_A;
			keyMap["B"] = DIK_B;
			keyMap["C"] = DIK_C;
			keyMap["D"] = DIK_D;
			keyMap["E"] = DIK_E;
			keyMap["F"] = DIK_F;
			keyMap["G"] = DIK_G;
			keyMap["H"] = DIK_H;
			keyMap["I"] = DIK_I;
			keyMap["J"] = DIK_J;
			keyMap["K"] = DIK_K;
			keyMap["L"] = DIK_L;
			keyMap["M"] = DIK_M;
			keyMap["N"] = DIK_N;
			keyMap["O"] = DIK_O;
			keyMap["P"] = DIK_P;
			keyMap["Q"] = DIK_Q;
			keyMap["R"] = DIK_R;
			keyMap["S"] = DIK_S;
			keyMap["T"] = DIK_T;
			keyMap["U"] = DIK_U;
			keyMap["V"] = DIK_V;
			keyMap["W"] = DIK_W;
			keyMap["X"] = DIK_X;
			keyMap["Y"] = DIK_Y;
			keyMap["Z"] = DIK_Z;
			keyMap["0"] = DIK_0;
			keyMap["1"] = DIK_1;
			keyMap["2"] = DIK_2;
			keyMap["3"] = DIK_3;
			keyMap["4"] = DIK_4;
			keyMap["5"] = DIK_5;
			keyMap["6"] = DIK_6;
			keyMap["7"] = DIK_7;
			keyMap["8"] = DIK_8;
			keyMap["9"] = DIK_9;
			keyMap["UP"] = DIK_UP;
			keyMap["DOWN"] = DIK_DOWN;
			keyMap["LEFT"] = DIK_LEFT;
			keyMap["RIGHT"] = DIK_RIGHT;
			keyMap["SPACE"] = DIK_SPACE;
			keyMap["RETURN"] = DIK_RETURN;
			keyMap["ESCAPE"] = DIK_ESCAPE;
			keyMap["RSHIFT"] = DIK_RSHIFT;
			keyMap["LSHIFT"] = DIK_LSHIFT;
			keyMap["BACKSLASH"] = DIK_BACKSLASH;
			keyMap["BACK"] = DIK_BACK;
			keyMap["TAB"] = DIK_TAB;
			keyMap["CAPITAL"] = DIK_CAPITAL;
			keyMap["SEMICOLON"] = DIK_SEMICOLON;
			keyMap["COMMA"] = DIK_COMMA;
			keyMap["PERIOD"] = DIK_PERIOD;
			keyMap["SLASH"] = DIK_SLASH;
			keyMap["LCONTROL"] = DIK_LCONTROL;
			keyMap["RCONTROL"] = DIK_RCONTROL;
			keyMap["LMENU"] = DIK_LMENU;
			keyMap["RMENU"] = DIK_RMENU;
			keyMap["LWIN"] = DIK_LWIN;
			keyMap["RWIN"] = DIK_RWIN;
			keyMap["APPS"] = DIK_APPS;
			keyMap["NUMPAD0"] = DIK_NUMPAD0;
			keyMap["NUMPAD1"] = DIK_NUMPAD1;
			keyMap["NUMPAD2"] = DIK_NUMPAD2;
			keyMap["NUMPAD3"] = DIK_NUMPAD3;
			keyMap["NUMPAD4"] = DIK_NUMPAD4;
			keyMap["NUMPAD5"] = DIK_NUMPAD5;
			keyMap["NUMPAD6"] = DIK_NUMPAD6;
			keyMap["NUMPAD7"] = DIK_NUMPAD7;
			keyMap["NUMPAD8"] = DIK_NUMPAD8;
			keyMap["NUMPAD9"] = DIK_NUMPAD9;
			keyMap["NUMPADENTER"] = DIK_NUMPADENTER;
			keyMap["NUMPADCOMMA"] = DIK_NUMPADCOMMA;
			keyMap["NUMPADPERIOD"] = DIK_NUMPADPERIOD;
			keyMap["NUMPADSLASH"] = DIK_NUMPADSLASH;
			keyMap["NUMPADSTAR"] = DIK_NUMPADSTAR;
			keyMap["NUMPADMINUS"] = DIK_NUMPADMINUS;
			keyMap["NUMPADPLUS"] = DIK_NUMPADPLUS;
			keyMap["NUMLOCK"] = DIK_NUMLOCK;
			keyMap["SCROLL"] = DIK_SCROLL;
			keyMap["F1"] = DIK_F1;
			keyMap["F2"] = DIK_F2;
			keyMap["F3"] = DIK_F3;
			keyMap["F4"] = DIK_F4;
			keyMap["F5"] = DIK_F5;
			keyMap["F6"] = DIK_F6;
			keyMap["F7"] = DIK_F7;
			keyMap["F8"] = DIK_F8;
			keyMap["F9"] = DIK_F9;
			keyMap["F10"] = DIK_F10;
			keyMap["F11"] = DIK_F11;
			keyMap["F12"] = DIK_F12;
			keyMap["F13"] = DIK_F13;
			keyMap["F14"] = DIK_F14;
			keyMap["F15"] = DIK_F15;

			return S_OK;
		}


		KeyboardInput()
		{
			win = nullptr;
			memset(&keys, 0, sizeof(keys));
			memset(&olds, 0, sizeof(olds));
		}

	public:
		~KeyboardInput()
		{

			if (key)
			{
				key->Unacquire();
				key->Release();
				key = nullptr;
			}
			if (input)
			{
				input->Release();
				input = nullptr;
			}

		}
		bool GetKey(const std::string &key) override;
		bool GetKeyDown(const std::string &key) override;
		bool GetKeyUp(const std::string &key) override;

		static KeyboardInput &GetInstance();

		void Initialize(HWND hWnd);

		void Update()
		{
			for (int i = 0; i < KEY_MAX; i++)
			{
				olds[i] = keys[i];
			}
		}

	};

}