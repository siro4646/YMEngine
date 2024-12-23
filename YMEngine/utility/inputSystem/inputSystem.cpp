//#include "inputSystem.h"
//
//
//
//HWND* InputClass::Mouse::win = nullptr;
//HRESULT InputClass::Mouse::result = S_OK;
//LPDIRECTINPUT8 InputClass::Mouse::input = nullptr;
//LPDIRECTINPUTDEVICE8 InputClass::Mouse::key = nullptr;
//DIMOUSESTATE2 InputClass::Mouse::cur{};
//DIMOUSESTATE2 InputClass::Mouse::prv{};
//POINT InputClass::Mouse::pt{};
//
//
//
//HWND* InputClass::KeyBoard::win = nullptr;
//HRESULT InputClass::KeyBoard::result = S_OK;
//LPDIRECTINPUT8 InputClass::KeyBoard::input = nullptr;
//LPDIRECTINPUTDEVICE8 InputClass::KeyBoard::key = nullptr;
//BYTE InputClass::KeyBoard::keys[] = {};
//BYTE InputClass::KeyBoard::olds[] = {};
//
//void InputClass::InputManager::Initialize(HWND hWnd)
//{
//	
//
//	System::InputSystem::KeyBoard.Initialize(hWnd);
//	System::InputSystem::Mouse.Initialize(hWnd);
//}
//
//void InputClass::InputManager::Update()
//{
//	System::InputSystem::KeyBoard.Update();
//
//	System::InputSystem::Mouse.Update();
//}
