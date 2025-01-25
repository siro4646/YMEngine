#include "imguiRender.h"


#include "application/application.h"
#include "winAPI/window/window.h"
#include "swapChain/swapChain.h"
#include "Renderer/renderer.h"
#include "device/device.h"


namespace ym
{
	void ImGuiRender::Init()
	{
		renderer = Renderer::Instance();
		device = renderer->GetDevice();
		swapChain = &device->GetSwapChain();		

		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 1;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		desc.NodeMask = 0;
		device->GetDeviceDep()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(pHeap.ReleaseAndGetAddressOf()));


		IMGUI_CHECKVERSION();

		if (ImGui::CreateContext() == nullptr)
		{
			ym::ConsoleLog("Failed to create ImGui context\n");
			assert(false);
			return;
		}

		ImGuiIO &io = ImGui::GetIO(); (void)io;

		bool result = ImGui_ImplWin32_Init(Application::Instance()->GetWindow()->GetWndHandle());
		if (!result)
		{
			ym::ConsoleLog("Failed to initialize ImGui Win32\n");
			assert(false);
			return;
		}
		result = ImGui_ImplDX12_Init(device->GetDeviceDep(), swapChain->kFrameCount,
			swapChain->GetCurrentTexture()->GetTextureDesc().format,
			pHeap.Get(),
			pHeap->GetCPUDescriptorHandleForHeapStart(),
			pHeap->GetGPUDescriptorHandleForHeapStart());
		if (!result)
		{
			ym::ConsoleLog("Failed to initialize ImGui DX12\n");
			assert(false);
			return;
		}
		isInit = true;
		
	}
	void ImGuiRender::Uninit()
	{
		if (!isInit)return;

		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		isInit = false;
	}
	void ImGuiRender::NewFrame()
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}
	void ImGuiRender::EndFrame()
	{
		ImGui::Render();
	}
	void ImGuiRender::Render(CommandList *cmdList)
	{
		auto descHeap = pHeap.Get();
		cmdList->GetCommandList()->SetDescriptorHeaps(1,&descHeap);
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList->GetCommandList());
	}
	void ImGuiRender::Draw()
	{
		/*ImGui::Begin("Hello, world!");
		ImGui::SetWindowSize(ImVec2(200, 100),ImGuiCond_::ImGuiCond_FirstUseEver);
		ImGui::End();


		ImGui::Begin("AAAAAAAAAAAAAAA");
		ImGui::SetWindowSize(ImVec2(500, 100), ImGuiCond_::ImGuiCond_FirstUseEver);
		ImGui::End();*/

	}

}