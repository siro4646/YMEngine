#include "keyBoardInput.h"

#include "application/application.h"
namespace ym 
{


	HWND *KeyboardInput::win = nullptr;
	HRESULT KeyboardInput::result = S_OK;
	LPDIRECTINPUT8 KeyboardInput::input = nullptr;
	LPDIRECTINPUTDEVICE8 KeyboardInput::key = nullptr;
	BYTE KeyboardInput::keys[KEY_MAX] = {};
	BYTE KeyboardInput::olds[KEY_MAX] = {};
	std::unordered_map<std::string, UINT> KeyboardInput::keyMap{};


    bool KeyboardInput::GetKey(const std::string &key)
	{
		if (!Application::Instance()->IsWindowFocus())
		{
			return false;
		}

		return keys[keyMap[key]] & 0x80;
	}

	bool KeyboardInput::GetKeyDown(const std::string &key)
	{
		bool isFocus = Application::Instance()->IsWindowFocus();
		if (!isFocus)
		{
			return false;
		}
		// ƒL[‚ª‰Ÿ‚³‚ê‚½uŠÔ‚ðŒŸo
		return (keys[keyMap[key]] & 0x80) && !(olds[keyMap[key]] & 0x80);
	}

    bool KeyboardInput::GetKeyUp(const std::string &key)
	{
		if (!Application::Instance()->IsWindowFocus())
		{
			return false;
		}

		return !(keys[keyMap[key]] & 0x80) && (olds[keyMap[key]] & 0x80);
	}



	KeyboardInput &KeyboardInput::Instance()
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