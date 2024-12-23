#include "C:\Users\yusuk\Desktop\study\制作\リファクタリング\リファクタリング\pch.h"
#include "gamePadInput.h"

namespace ym {

	bool GamePadInput::GetKey(const std::string &key)
	{
		XInputGetState(0, &state);
		return (state.Gamepad.wButtons & keyMap[key]) ? true : false;
	}

	bool GamePadInput::GetKeyDown(const std::string &key)
	{
		XInputGetState(0, &state);
		return (state.Gamepad.wButtons & keyMap[key]) && !down[keyMap[key]];
	}

	bool GamePadInput::GetKeyUp(const std::string &key)
	{
		XInputGetState(0, &state);
		return !(state.Gamepad.wButtons & keyMap[key]) && down[keyMap[key]];
	}

	GamePadInput &GamePadInput::GetInstance()
	{
		static GamePadInput instance;
		return instance;
	}

}