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
		/// ウィンドウハンドルの取得
		/// </summary>
		/// <returns>ウィンドウハンドル</returns>
		HWND GetWndHandle() const { return m_hWnd; }

		/// <summary>
		/// ウィンドウの作成
		/// </summary>
		/// <param name="clientWidth">横幅</param>
		/// <param name="clientHeight">縦幅</param>
		/// <param name="titleName">タイトル名</param>
		/// <param name="windowClassName">クラス名</param>
		/// <returns>成功したらtrue</returns>
		bool Create(int clientWidth, int clientHeight, const std::string &titleName, const std::string &windowClassName);

		/// <summary>
		/// ウィンドウメッセージ処理
		/// </summary>
		/// <returns>終了メッセージが来たらfalseが来る</returns>
		bool ProcessMessage();

		/// <summary>
		/// ウィンドウの名前を変更する
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
		HWND	m_hWnd;		// ウィンドウハンドル
	};
}
