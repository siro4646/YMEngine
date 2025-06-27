#pragma once

namespace ym
{
	class Window
	{
	public:

		u32 GetWndWidth()
		{
			tagRECT rect;
			GetWindowRect(m_hWnd, &rect);
			return rect.right - rect.left;
		}
		u32 GetWndHeight()
		{
			tagRECT rect;
			GetWindowRect(m_hWnd, &rect);
			return rect.bottom - rect.top;
		}
		/// <summary>
		/// �E�B���h�E�n���h���̎擾
		/// </summary>
		/// <returns>�E�B���h�E�n���h��</returns>
		HWND GetWndHandle() const { return m_hWnd; }

		/// <summary>
		/// �E�B���h�E�̍쐬
		/// </summary>
		/// <param name="clientWidth">����</param>
		/// <param name="clientHeight">�c��</param>
		/// <param name="titleName">�^�C�g����</param>
		/// <param name="windowClassName">�N���X��</param>
		/// <returns>����������true</returns>
		bool Create(int clientWidth, int clientHeight, const std::string &titleName, const std::string &windowClassName);

		/// <summary>
		/// �E�B���h�E���b�Z�[�W����
		/// </summary>
		/// <returns>�I�����b�Z�[�W��������false������</returns>
		bool ProcessMessage();

		/// <summary>
		/// �E�B���h�E�̖��O��ύX����
		/// </summary>
		void SetWndName(const std::string &name)
		{
			SetWindowTextA(m_hWnd, name.c_str());
		}

		void Show()
		{
			ShowWindow(m_hWnd, SW_SHOW);
			UpdateWindow(m_hWnd);
		}
		void SetWindowFont(HWND hwnd) {
			HFONT hFont = CreateFontW(
				20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
				SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Meiryo");
			SendMessage(hwnd, WM_SETFONT, (WPARAM)hFont, TRUE);
		}

		inline bool IsWindowFocus() const
		{
			return GetForegroundWindow() == m_hWnd;
		}


	private:
		HWND	m_hWnd;		// �E�B���h�E�n���h��
	};
}
