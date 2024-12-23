#pragma once

#include "../base/inputDevice.h"
#include "../keyBoard/keyBoardInput.h"
#include "../gamePad/gamePadInput.h"
#include "../mouse/mouseInput.h"

namespace ym {

	class InputManager
	{
	private:
		class InputAction
		{
		public:
			std::shared_ptr<InputDevice> device;
			std::string key;
		};
		// アクションとその対応するデバイス入力のマッピング
		std::unordered_map<std::string, std::vector<InputAction>> m_actionMap;

		InputManager() = default;

	public:
		// アクション登録
		void RegisterAction(const std::string &action, std::shared_ptr<InputDevice> device, const std::string &key) {
			m_actionMap[action].push_back({ device, key });
		}

		bool GetAction(const std::string &action) {
			auto it = m_actionMap.find(action);
			if (it != m_actionMap.end()) {
				for (const auto &action : it->second) {
					if (action.device->GetKey(action.key)) {
						return true;
					}
				}
			}
			return false;
		}
		bool GetActionDown(const std::string &action) {
			auto it = m_actionMap.find(action);
			if (it != m_actionMap.end()) {
				for (const auto &action : it->second) {
					if (action.device->GetKeyDown(action.key)) {
						return true;
					}
				}
			}
			return false;
		}
		bool GetActionUp(const std::string &action) {
			auto it = m_actionMap.find(action);
			if (it != m_actionMap.end()) {
				for (const auto &action : it->second) {
					if (action.device->GetKeyUp(action.key)) {
						return true;
					}
				}
			}
			return false;
		}

		static InputManager &GetInstance() {
			static InputManager instance;
			return instance;
		}

		void Initialize(HWND hWnd) {
			KeyboardInput::GetInstance().Initialize(hWnd);
			MouseInput::GetInstance().Initialize(hWnd);
		}

		void Update() {
			KeyboardInput::GetInstance().Update();
			MouseInput::GetInstance().Update();

		}

	};
}