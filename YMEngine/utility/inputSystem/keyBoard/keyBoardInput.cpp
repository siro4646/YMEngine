#include "keyBoardInput.h"

namespace ym 
{


	HWND *KeyboardInput::win = nullptr;
	HRESULT KeyboardInput::result = S_OK;
	LPDIRECTINPUT8 KeyboardInput::input = nullptr;
	LPDIRECTINPUTDEVICE8 KeyboardInput::key = nullptr;
	BYTE KeyboardInput::keys[] = {};
	BYTE KeyboardInput::olds[] = {};
	std::unordered_map<std::string, UINT> KeyboardInput::keyMap{};


	bool KeyboardInput::GetKey(const std::string &key)
	{
		//キー情報を取得
		this->key->GetDeviceState(sizeof(keys), &keys);

		return keys[keyMap[key]] & 0x80;
	}

	bool KeyboardInput::GetKeyDown(const std::string &key)
	{
		//キー情報を取得
		this->key->GetDeviceState(sizeof(keys), &keys);

		return (keys[keyMap[key]] & 0x80) && !(olds[keyMap[key]] & 0x80);
	}

	bool KeyboardInput::GetKeyUp(const std::string &key)
	{

		//キー情報を取得
		this->key->GetDeviceState(sizeof(keys), &keys);
		return !(keys[keyMap[key]] & 0x80) && (olds[keyMap[key]] & 0x80);
	}


	KeyboardInput &KeyboardInput::GetInstance()
	{
		static KeyboardInput instance;
		return instance;
	}

	void KeyboardInput::Initialize(HWND hWnd)
	{
		if (key)return;
		win = &hWnd;
		memset(&keys, 0, sizeof(keys));
		memset(&olds, 0, sizeof(olds));

		if (S_OK == CreateInput())
		{

		}
		if (S_OK == CreateKey())
		{

		}
		if (S_OK == SetKeyFormat())
		{

		}
		if (S_OK == SetKeyCooperative())
		{

		}
		if (S_OK == CreateKeyMap())
		{

		}
	}
}