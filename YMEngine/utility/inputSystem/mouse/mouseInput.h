#pragma once
#include "../base/inputDevice.h"
#include "../../../utility/vector/vector2.h"
#include <unordered_map>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#pragma comment (lib, "dinput8.lib")
#pragma comment(lib, "DXGUID.LIB")

namespace ym
{


	class MouseInput : public InputDevice {

	private:

		HRESULT CreateInput()
		{
			result = DirectInput8Create(GetModuleHandle(0), DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)(&input),
				NULL);

			return result;
		}
		HRESULT CreateMouse(void)
		{
			result = input->CreateDevice(GUID_SysMouse, &key, NULL);

			return result;
		}
		HRESULT SetMouseFormat(void)
		{
			result = key->SetDataFormat(&c_dfDIMouse2);

			return result;
		}
		HRESULT SetKeyCooperative(void)
		{
			result = key->SetCooperativeLevel(*win, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

			//���̓f�o�C�X�ւ̃A�N�Z�X�������擾
			key->Acquire();
			return result;
		}
		HRESULT CreateKeyMap()
		{
			//������
			keyMap["left"] = 0;
			keyMap["right"] = 1;
			keyMap["middle"] = 2;
			keyMap["side1"] = 3;
			keyMap["side2"] = 4;

			//�啶��
			keyMap["LEFT"] = 0;
			keyMap["RIGHT"] = 1;
			keyMap["MIDDLE"] = 2;
			keyMap["SIDE1"] = 3;
			keyMap["SIDE2"] = 4;
			//�擪�����啶��
			keyMap["Left"] = 0;
			keyMap["Right"] = 1;
			keyMap["Middle"] = 2;
			keyMap["Side1"] = 3;
			keyMap["Side2"] = 4;
			return S_OK;

		}

		MouseInput()
		{

		}



		// �E�B���h�E
		static HWND *win;
		// �Q�ƌ���
		static HRESULT result;
		// �C���v�b�g
		static LPDIRECTINPUT8 input;
		// �C���v�b�g�f�o�C�X
		static LPDIRECTINPUTDEVICE8 key;

		static DIMOUSESTATE2 cur, prv;

		static POINT pt;

		// string����L�[���ɕϊ��p�}�b�v
		static std::unordered_map<std::string, UINT> keyMap;

	public:
		~MouseInput()
		{
			if (key)return;
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

		Vector2 GetMousePos()
		{
			GetCursorPos(&pt);
			ScreenToClient(*win, &pt);
			return Vector2(pt.x, pt.y);
		}
		Vector2 GetMouseAcc()
		{
			//�L�[�����擾
			this->key->Poll();
			this->key->GetDeviceState(sizeof(cur), &cur);
			return Vector2(cur.lX, cur.lY);
		}


		static MouseInput &GetInstance()
		{
			static MouseInput instance;
			return instance;

		}

		void Initialize(HWND hWnd);

		void Update()
		{
			for (int i = 0; i < 8; i++)
			{
				prv.rgbButtons[i] = cur.rgbButtons[i];
			}
		}


	};
}