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
		// ���b�Z�[�W���ɏ�����I��
		switch (message)
		{


		case WM_DESTROY:

			PostQuitMessage(0);		// OS�ɑ΂��ďI����`����
			break;
		case WM_KEYDOWN:
			switch (wParam)
			{
			case VK_ESCAPE:					// [ESC]�L�[�������ꂽ
				PostQuitMessage(0);		// OS�ɑ΂��ďI����`����
				DestroyWindow(hWnd);		// �E�B���h�E��j������悤�w������

				break;
			case VK_SPACE:
				//PIXGpuCaptureNextFrames(L"TestGPUCapture.wpix", 1);
				break;
			}
			break;
		//case WM_SIZE:
		//	if (wParam != SIZE_MINIMIZED) // �ŏ������͖���
		//	{
		//		UINT newWidth = LOWORD(lParam);
		//		UINT newHeight = HIWORD(lParam);
		//		if (newWidth > 0 && newHeight > 0)
		//		{
		//			if (Renderer::Instance()->GetDevice())
		//			{


		//				Renderer::Instance()->GetDevice()->GetSwapChain().Resize(Application::Instance()->GetWindow()->GetWndHandle()); // �������ŌĂ�
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
				DropFileManager::Instance().AddDroppedFile(std::string(filePath)); // �h���b�v���ꂽ�t�@�C���̃p�X��ۑ�
				// �����Ńt�@�C���p�X���g����BImGui�ɓn���Ȃ�ۑ����Č�ŕ`�掞�Ɏg��
				//droppedFiles.push_back(filePath); // std::vector<std::string> �Ȃǂɕۑ�
			}

			DragFinish(hDrop); // ���
			break;
		}


		default:
			return DefWindowProc(hWnd, message, wParam, lParam);		// ���b�Z�[�W�̃f�t�H���g����
			break;
		}

		return 0;
	}

	bool Window::Create(int clientWidth, int clientHeight, const std::string &titleName, const std::string &windowClassName)
	{
		// �C���X�^���X�n���h���擾
		HINSTANCE hInst = GetModuleHandle(0);

		//=====================================
		// ���C���E�B���h�E�쐬
		//=====================================

		// �E�B���h�E�N���X�̒�`
		WNDCLASSEXA wc = {};
		wc.style = CS_VREDRAW | CS_HREDRAW | CS_NOCLOSE;
		wc.hCursor = LoadCursor(NULL, IDC_NO);
		wc.hbrBackground = NULL;
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.cbSize = sizeof(WNDCLASSEXA);								// �\���̂̃T�C�Y
		wc.lpfnWndProc = (WNDPROC)WindowProcedure;					// �E�C���h�E�֐�
		wc.lpszClassName = windowClassName.c_str();					// �E�B���h�E�N���X��
		wc.hInstance = hInst;										// �C���X�^���X�n���h��

		// �E�B���h�E�N���X�̓o�^
		if (!RegisterClassExA(&wc))
		{
			DWORD error = GetLastError(); // �G���[�R�[�h���擾
			std::wcerr << L"RegisterClassEx failed with error code: " << error << std::endl;
			return false;
		}

		// �E�B���h�E�̍쐬
		m_hWnd = CreateWindowA(
			windowClassName.c_str(),							// �E�B���h�E�N���X��
			titleName.c_str(),									// �E�B���h�E�̃^�C�g��
			WS_OVERLAPPEDWINDOW| WS_MAXIMIZE,				// �E�B���h�E�^�C�v��W���^�C�v��	
			0,													// �E�B���h�E�̈ʒu�i�w���W�j
			0,													// �E�B���h�E�̈ʒu�i�x���W�j						
			clientWidth,										// �E�B���h�E�̕�
			clientHeight,										// �E�B���h�E�̍���			
			nullptr,											// �e�E�B���h�E�̃n���h��
			nullptr,											// ���j���[�̃n���h��
			hInst,												// �C���X�^���X�n���h�� 
			this
		);

		if (m_hWnd == nullptr)
		{
			return false;
		}
		
		SetWindowFont(m_hWnd);

		// �E�B���h�E�̕\��
		ShowWindow(m_hWnd, SW_SHOW);
		//�E�B���h�E�̍X�V
		UpdateWindow(m_hWnd);

		return true;
	}

	bool Window::ProcessMessage()
	{
		// ���b�Z�[�W�擾
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			// �I�����b�Z�[�W������
			if (msg.message == WM_QUIT) {
				return false;
			}

			//���b�Z�[�W����
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return true;
	}

}