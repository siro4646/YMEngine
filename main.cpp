#include "application/application.h"
#ifdef _DEBUG
int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);	// ���������[�N��m�点��
	ym::EnableDebugLayer();
#else
#include<Windows.h>
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#endif
	//_CrtSetBreakAlloc(292);
#ifdef _DEBUG

#endif _DEBUG

	using namespace ym;


	Application::Instance()->Init();
	Application::Instance()->Run();
	Application::Instance()->Terminate();
	//Application::Instance()->Delete();

//#ifdef _DEBUG
//	ComPtr<IDXGIDebug1>dxgiDebug;
//	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug)))) {
//		dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
//		//dxgiDebug->Release();
//	}
//#endif


	return 0;
}

