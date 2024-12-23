//#pragma once
////####################################################################
////使い方
////using namespace System::InputSystem;と記述 #ヘッダーファイルには書かないでください
////ゲームパッドならgamePad.～でいろいろ指定でき,padCode.～でキー指定できる
////キーボードはkeyBoard.～ keyCode.でキー指定 
////
////
////####################################################################
//
//#include <Windows.h>
////ゲームパッド用
//#include <Xinput.h>
//#pragma comment (lib, "Xinput.lib")
//
////キーボード用
//#define DIRECTINPUT_VERSION 0x0800
//#include <dinput.h>
//
//#pragma comment (lib, "dinput8.lib")
//#pragma comment(lib, "DXGUID.LIB")
//// キー最大数
//#define KEY_MAX 256
//
//#include <iostream>
//
//#include "../../utility/vector/vector2.h"
//
//
//namespace InputClass {
//	class InputManager
//	{
//	public:
//		InputManager()
//		{
//
//		}
//		~InputManager()
//		{
//
//		}
//		void Initialize(HWND hWnd);
//
//		void Update();
//	private:
//	};
//	class GamePad
//	{
//	private:
//		XINPUT_STATE state;
//		XINPUT_VIBRATION vibration;
//
//	public:
//		GamePad()
//		{
//			//コントローラーの初期化
//			{
//				for (int i = 0; i < 16; i++)
//				{
//					up[i] = true;
//					down[i] = false;
//				}
//				vibration.wLeftMotorSpeed = 0;
//				vibration.wRightMotorSpeed = 0;
//			}
//		}
//
//		~GamePad()
//		{
//			vibration.wLeftMotorSpeed = 0;
//			vibration.wRightMotorSpeed = 0;
//			XInputSetState(0, &vibration);
//		}
//
//
//		bool up[16];
//		bool down[16];
//
//
//		bool GetKey(int key)
//		{
//			XInputGetState(0, &state);
//			return (state.Gamepad.wButtons & key) ? true : false;
//		}
//
//		bool GetKeyPress(int key)
//		{
//			int index = (int)log2f(key);
//			if (GetKey(key))
//			{
//				up[index] = false;
//				if (!down[index])
//				{
//					down[index] = true;
//					return true;
//				}
//				return false;
//			}
//			down[index] = false;
//			up[index] = true;
//			return false;
//		}
//
//		bool GetKeyRelease(int key)
//		{
//			int index = (int)log2f(key);
//			if (GetKey(key))
//			{
//				up[index] = false;
//				if (!down[index])
//				{
//					down[index] = true;
//					return false;
//				}
//				return false;
//			}
//			if (!GetKey(key))
//			{
//				if (down[index])
//				{
//					down[index] = false;
//					up[index] = true;
//					return true;
//				}
//			}
//			return false;
//		}
//
//
//		void GetLeftStickValue(float* x, float* y)
//		{
//			XInputGetState(0, &state);
//			if ((state.Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && state.Gamepad.sThumbLX > -
//				XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
//				(state.Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && state.Gamepad.sThumbLY > -
//					XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
//			{
//				state.Gamepad.sThumbLX = 0;
//				state.Gamepad.sThumbLY = 0;
//			}
//			*x = state.Gamepad.sThumbLX;
//			*y = state.Gamepad.sThumbLY;
//		}
//
//		void GetRightStickValue(float* x, float* y)
//		{
//			XInputGetState(0, &state);
//			if ((state.Gamepad.sThumbRX < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && state.Gamepad.sThumbRX > -
//				XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
//				(state.Gamepad.sThumbRY < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && state.Gamepad.sThumbRY > -
//					XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
//			{
//				state.Gamepad.sThumbRX = 0;
//				state.Gamepad.sThumbRY = 0;
//			}
//			*x = state.Gamepad.sThumbRX;
//			*y = state.Gamepad.sThumbRY;
//		}
//
//		int LeftTriggerValue()
//		{
//			XInputGetState(0, &state);
//			return state.Gamepad.bLeftTrigger;
//		}
//
//		int RightTriggerValue()
//		{
//			XInputGetState(0, &state);
//			return state.Gamepad.bRightTrigger;
//		}
//		void LeftVibration(float index)
//		{
//			XInputSetState(0, &vibration);
//			vibration.wLeftMotorSpeed = index;
//		}
//		void RightVibration(float index)
//		{
//			vibration.wRightMotorSpeed = index;
//			XInputSetState(0, &vibration);
//		}
//	};
//	class KeyBoard
//	{
//	public:
//
//		void Initialize(HWND hWnd)
//		{
//			if (key)return;
//			win = &hWnd;
//			memset(&keys, 0, sizeof(keys));
//			memset(&olds, 0, sizeof(olds));
//
//			if (S_OK == CreateInput())
//			{
//
//			}
//			if (S_OK == CreateKey())
//			{
//
//			}
//			if (S_OK == SetKeyFormat())
//			{
//
//			}
//			if (S_OK == SetKeyCooperative())
//			{
//
//			}
//			;
//			;
//		}
//
//		KeyBoard()
//		{
//			win = nullptr;
//			memset(&keys, 0, sizeof(keys));
//			memset(&olds, 0, sizeof(olds));
//		}
//
//		~KeyBoard()
//		{
//			//key->Unacquire();
//
//			if (key)
//			{
//				key->Unacquire();
//				key->Release();
//				key = nullptr;
//			}
//			if (input)
//			{
//				input->Release();
//				input = nullptr;
//			}
//			//delete win;
//		}
//
//		// キー入力
//		bool CheckKey(UINT index)
//		{
//			//チェックフラグ
//			bool flag = false;
//
//			//キー情報を取得
//			key->GetDeviceState(sizeof(keys), &keys);
//			if (keys[index] & 0x80)
//			{
//				flag = true;
//			}
//			//olds[index] = keys[index];
//
//			return flag;
//		}
//
//		// トリガーの入力
//		bool TriggerKey(UINT index)
//		{
//			//チェックフラグ
//			bool flag = false;
//
//			//キー情報を取得
//			key->GetDeviceState(sizeof(keys), &keys);
//			if ((keys[index] & 0x80) && !(olds[index] & 0x80))
//			{
//				flag = true;
//			}
//			//olds[index] = keys[index];
//
//			return flag;
//		}
//		void Update()
//		{
//			for (int i = 0; i < KEY_MAX; i++)
//			{
//				olds[i] = keys[i];
//			}
//		}
//	private:
//		// インプットの生成
//		HRESULT CreateInput(void)
//		{
//			result = DirectInput8Create(GetModuleHandle(0), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)(&input),
//				NULL);
//
//			return result;
//		}
//
//		// キーデバイスの生成
//		HRESULT CreateKey(void)
//		{
//			result = input->CreateDevice(GUID_SysKeyboard, &key, NULL);
//
//			return result;
//		}
//
//
//		// キーフォーマットのセット
//		HRESULT SetKeyFormat(void)
//		{
//			result = key->SetDataFormat(&c_dfDIKeyboard);
//
//			return result;
//		}
//		// キーの協調レベルのセット
//		HRESULT SetKeyCooperative(void)
//		{
//			result = key->SetCooperativeLevel(*win, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
//
//			//入力デバイスへのアクセス権利を取得
//			key->Acquire();
//
//			return result;
//		}
//	public:
//		// ウィンドウ
//		static HWND* win;
//		// 参照結果
//		static HRESULT result;
//		// インプット
//		static LPDIRECTINPUT8 input;
//		// インプットデバイス
//		static LPDIRECTINPUTDEVICE8 key;
//		// キー情報
//		static BYTE keys[KEY_MAX];
//		// 前のキー情報
//		static BYTE olds[KEY_MAX];
//	};
//	class Mouse
//	{
//	public:
//		void Initialize(HWND hWnd)
//		{
//			win = &hWnd;
//			if (!S_OK == CreateInput())
//			{
//				return;
//			}
//			if (!S_OK == CreateMouse())
//			{
//				return;
//
//
//			}
//			if (!S_OK == SetMouseFormat())
//			{
//				return;
//
//			}
//			if (!S_OK == SetKeyCooperative())
//			{
//				return;
//
//			}
//		}
//		~Mouse()
//		{
//			if (key)return;
//			if (key)
//			{
//				key->Unacquire();
//				key->Release();
//				key = nullptr;
//			}
//			if (input)
//			{
//				input->Release();
//				input = nullptr;
//			}
//		}
//
//		bool GetMouseButton(int keyNumber)
//		{
//			//ポーリング開始
//			key->Poll();
//
//			//チェックフラグ
//			bool flag = false;
//
//			//キー情報を取得
//			key->GetDeviceState(sizeof(DIMOUSESTATE2), &cur);
//			if ((cur.rgbButtons[keyNumber] & 0x80))
//			{
//				flag = true;
//			}
//			//prv.rgbButtons[keyNumber] = cur.rgbButtons[keyNumber];
//
//			return flag;
//
//		}
//		bool GetMouseButtonDown(int keyNumber)
//		{
//			//ポーリング開始
//			key->Poll();
//
//			//チェックフラグ
//			bool flag = false;
//
//			//キー情報を取得
//			key->GetDeviceState(sizeof(DIMOUSESTATE2), &cur);
//			if ((cur.rgbButtons[keyNumber] & 0x80) && !(prv.rgbButtons[keyNumber] & 0x80))
//			{
//				flag = true;
//			}
//			//prv.rgbButtons[keyNumber] = cur.rgbButtons[keyNumber];
//
//			return flag;
//		}
//		bool GetMouseButtonUp(int keyNumber)
//		{
//			//ポーリング開始
//			key->Poll();
//
//			//チェックフラグ
//			bool flag = false;
//
//			//キー情報を取得
//			key->GetDeviceState(sizeof(DIMOUSESTATE2), &cur);
//			if (!(cur.rgbButtons[keyNumber] & 0x80) && (prv.rgbButtons[keyNumber] & 0x80))
//			{
//				flag = true;
//			}
//
//			//prv.rgbButtons[keyNumber] = cur.rgbButtons[keyNumber];
//
//			return flag;
//		}
//		Vector2 GetMouseAcceleration()
//		{
//			//ポーリング開始
//			key->Poll();
//
//
//
//			key->GetDeviceState(sizeof(DIMOUSESTATE2), &cur);
//			return Vector2(cur.lX, cur.lY);
//		}
//		Vector2 GetMousePos()
//		{
//			GetCursorPos(&pt);
//			return Vector2(pt.x, pt.y);
//		}
//		void Update()
//		{
//			for (int i = 0; i < 8; i++)
//			{
//				prv.rgbButtons[i] = cur.rgbButtons[i];
//			}
//		}
//	private:
//		HRESULT CreateInput()
//		{
//			result = DirectInput8Create(GetModuleHandle(0), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)(&input),
//				NULL);
//
//			return result;
//		}
//		HRESULT CreateMouse(void)
//		{
//			result = input->CreateDevice(GUID_SysMouse, &key, NULL);
//
//			return result;
//		}
//		HRESULT SetMouseFormat(void)
//		{
//			result = key->SetDataFormat(&c_dfDIMouse2);
//
//			return result;
//		}
//		HRESULT SetKeyCooperative(void)
//		{
//			result = key->SetCooperativeLevel(*win, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
//
//			//入力デバイスへのアクセス権利を取得
//			key->Acquire();
//			return result;
//		}
//
//
//
//		// ウィンドウ
//		static HWND* win;
//		// 参照結果
//		static HRESULT result;
//		// インプット
//		static LPDIRECTINPUT8 input;
//		// インプットデバイス
//		static LPDIRECTINPUTDEVICE8 key;
//
//		static DIMOUSESTATE2 cur, prv;
//
//		static POINT pt;
//
//
//	};
//
//
//	class KeyCode
//	{
//	public:
//		enum
//		{
//			Q = DIK_Q,
//			W,
//			E,
//			R,
//			T,
//			Y,
//			U,
//			I,
//			O,
//			P,
//			A = DIK_A,
//			S,
//			D,
//			F,
//			G,
//			H,
//			J,
//			K,
//			L,
//			RShift = DIK_RSHIFT,
//			LShift = DIK_LSHIFT,
//			BackSlash,
//			Z = DIK_Z,
//			X,
//			C,
//			V,
//			B,
//			N,
//			M,
//			Space = DIK_SPACE,
//			Up = DIK_UP,
//			Down = DIK_DOWN,
//			Left = DIK_LEFT,
//			Right = DIK_RIGHT,
//		};
//	};
//
//	class MouseCode
//	{
//	public:
//		enum {
//			Left = 0,
//			Right = 1,
//			Midle = 2,
//			Side1 = 3,
//			Side2 = 4,
//		};
//	};
//
//	class PadCode
//	{
//		class Cross
//		{
//		public:
//			enum
//			{
//				Up = XINPUT_GAMEPAD_DPAD_UP,
//				Down = XINPUT_GAMEPAD_DPAD_DOWN,
//				Left = XINPUT_GAMEPAD_DPAD_LEFT,
//				Right = XINPUT_GAMEPAD_DPAD_RIGHT,
//			};
//		};
//		class Button
//		{
//		public:
//			enum
//			{
//				Up = XINPUT_GAMEPAD_Y,
//				Down = XINPUT_GAMEPAD_A,
//				Left = XINPUT_GAMEPAD_X,
//				Right = XINPUT_GAMEPAD_B,
//				L = XINPUT_GAMEPAD_LEFT_SHOULDER,
//				R = XINPUT_GAMEPAD_RIGHT_SHOULDER,
//				Start = XINPUT_GAMEPAD_START,
//				Option = XINPUT_GAMEPAD_BACK,
//				LStick = XINPUT_GAMEPAD_LEFT_THUMB,
//				RStick = XINPUT_GAMEPAD_RIGHT_THUMB,
//			};
//		};
//
//	public:
//		Cross cross;
//		Button button;
//
//	};
//}
//
//
//
//namespace System
//{
//
//
//	namespace InputSystem
//	{
//		static InputClass::InputManager InputManager;
//
//		static InputClass::Mouse Mouse;
//		static InputClass::MouseCode MouseCode;
//
//		static InputClass::KeyBoard KeyBoard;
//		static InputClass::KeyCode KeyCode;
//		static InputClass::GamePad GamePad;
//		static InputClass::PadCode PadCode;
//	}
//}
