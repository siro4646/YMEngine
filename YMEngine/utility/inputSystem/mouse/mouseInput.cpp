#include "mouseInput.h"

namespace ym {

	HWND *MouseInput::win = nullptr;
	HRESULT MouseInput::result = S_OK;
	LPDIRECTINPUT8 MouseInput::input = nullptr;
	LPDIRECTINPUTDEVICE8 MouseInput::key = nullptr;
	DIMOUSESTATE2 MouseInput::cur{};
	DIMOUSESTATE2 MouseInput::prv{};
	POINT MouseInput::pt{};
	std::unordered_map<std::string, UINT> MouseInput::keyMap{};

	bool MouseInput::GetKey(const std::string &key)
	{
		//キー情報を取得
		this->key->Poll();
		this->key->GetDeviceState(sizeof(cur), &cur);

		return cur.rgbButtons[keyMap[key]] & 0x80;
	}

	bool MouseInput::GetKeyDown(const std::string &key)
	{
		//キー情報を取得
		this->key->Poll();
		this->key->GetDeviceState(sizeof(cur), &cur);

		return (cur.rgbButtons[keyMap[key]] & 0x80) && !(prv.rgbButtons[keyMap[key]] & 0x80);
	}

	bool MouseInput::GetKeyUp(const std::string &key)
	{
		//キー情報を取得
		this->key->Poll();
		this->key->GetDeviceState(sizeof(cur), &cur);

		return !(cur.rgbButtons[keyMap[key]] & 0x80) && (prv.rgbButtons[keyMap[key]] & 0x80);
	}

	void MouseInput::Initialize(HWND hWnd)
	{
		win = &hWnd;
		if (!S_OK == CreateInput())
		{
			return;
		}
		if (!S_OK == CreateMouse())
		{
			return;


		}
		if (!S_OK == SetMouseFormat())
		{
			return;

		}
		if (!S_OK == SetKeyCooperative())
		{
			return;

		}

		if (!S_OK == CreateKeyMap())
		{
			return;
		}
	}

}