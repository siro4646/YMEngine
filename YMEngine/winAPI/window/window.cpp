//pch.h is included in window.h
#include "window.h"

#include "Renderer/renderer.h"

	extern LRESULT ImGui_ImplWin32_WndProcHandler(
		HWND, UINT, WPARAM, LPARAM);
namespace ym {


	LRESULT WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{

		if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		// メッセージ毎に処理を選択
		switch (message)
		{


		case WM_DESTROY:

			PostQuitMessage(0);		// OSに対して終了を伝える
			break;
		case WM_KEYDOWN:
			switch (wParam)
			{
			case VK_ESCAPE:					// [ESC]キーが押された
				PostQuitMessage(0);		// OSに対して終了を伝える
				DestroyWindow(hWnd);		// ウィンドウを破棄するよう指示する

				break;
			case VK_SPACE:
				//PIXGpuCaptureNextFrames(L"TestGPUCapture.wpix", 1);
				break;
			}
			break;
		//case WM_SIZE:
		//	if (wParam != SIZE_MINIMIZED) // 最小化時は無視
		//	{
		//		UINT newWidth = LOWORD(lParam);
		//		UINT newHeight = HIWORD(lParam);
		//		if (newWidth > 0 && newHeight > 0)
		//		{
		//			if (Renderer::Instance()->GetDevice())
		//			{


		//				Renderer::Instance()->GetDevice()->GetSwapChain().Resize(Application::Instance()->GetWindow()->GetWndHandle()); // ←ここで呼ぶ
		//			}
		//		}
		//	}
		//	break;
		case WM_DROPFILES:
		{
			HDROP hDrop = (HDROP)wParam;
			UINT fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);

			for (UINT i = 0; i < fileCount; ++i) {
				TCHAR filePath[MAX_PATH];
				DragQueryFile(hDrop, i, filePath, MAX_PATH);
				DropFileManager::Instance().AddDroppedFile(std::string(filePath)); // ドロップされたファイルのパスを保存
				// ここでファイルパスを使える。ImGuiに渡すなら保存して後で描画時に使う
				//droppedFiles.push_back(filePath); // std::vector<std::string> などに保存
			}

			DragFinish(hDrop); // 解放
			break;
		}


		default:
			return DefWindowProc(hWnd, message, wParam, lParam);		// メッセージのデフォルト処理
			break;
		}

		return 0;
	}

	bool Window::Create(int clientWidth, int clientHeight, const std::string &titleName, const std::string &windowClassName)
	{
		// インスタンスハンドル取得
		HINSTANCE hInst = GetModuleHandle(0);

		//=====================================
		// メインウィンドウ作成
		//=====================================

		// ウィンドウクラスの定義
		WNDCLASSEXA wc = {};
		wc.style = CS_VREDRAW | CS_HREDRAW | CS_NOCLOSE;
		wc.hCursor = LoadCursor(NULL, IDC_NO);
		wc.hbrBackground = NULL;
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.cbSize = sizeof(WNDCLASSEXA);								// 構造体のサイズ
		wc.lpfnWndProc = (WNDPROC)WindowProcedure;					// ウインドウ関数
		wc.lpszClassName = windowClassName.c_str();					// ウィンドウクラス名
		wc.hInstance = hInst;										// インスタンスハンドル

		// ウィンドウクラスの登録
		if (!RegisterClassExA(&wc))
		{
			DWORD error = GetLastError(); // エラーコードを取得
			std::wcerr << L"RegisterClassEx failed with error code: " << error << std::endl;
			return false;
		}

		// ウィンドウの作成
		m_hWnd = CreateWindowA(
			windowClassName.c_str(),							// ウィンドウクラス名
			titleName.c_str(),									// ウィンドウのタイトル
			WS_OVERLAPPEDWINDOW| WS_MAXIMIZE,				// ウィンドウタイプを標準タイプに	
			0,													// ウィンドウの位置（Ｘ座標）
			0,													// ウィンドウの位置（Ｙ座標）						
			clientWidth,										// ウィンドウの幅
			clientHeight,										// ウィンドウの高さ			
			nullptr,											// 親ウィンドウのハンドル
			nullptr,											// メニューのハンドル
			hInst,												// インスタンスハンドル 
			this
		);

		if (m_hWnd == nullptr)
		{
			return false;
		}
		
		SetWindowFont(m_hWnd);

		// ウィンドウの表示
		ShowWindow(m_hWnd, SW_SHOW);
		//ウィンドウの更新
		UpdateWindow(m_hWnd);

		return true;
	}

	bool Window::ProcessMessage()
	{
		// メッセージ取得
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			// 終了メッセージがきた
			if (msg.message == WM_QUIT) {
				return false;
			}

			//メッセージ処理
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return true;
	}

}