#pragma once

namespace ym
{
	class Renderer;
	class Device;
	class SwapChain;

	class Texture;
	class TextureView;
	class Buffer;
	class ConstantBufferView;
	class VertexBufferView;
	class IndexBufferView;
	class Shader;
	class Sampler;
	class CommandList;
	class RootSignature;
	class GraphicsPipelineState;
	class DescriptorSet;

	struct SRVHandlePair {
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
	};

	// imguiRender.h
	struct PendingImage
	{
		Texture *texture;
		ImVec2 size;
		ImVec2 uv0;
		ImVec2 uv1;
		ImVec4 tintColor;
		ImVec4 borderColor;
	};


	class ImGuiRender
	{
	public:
		void Init();
		void Uninit();
		static ImGuiRender *Instance()
		{
			static ImGuiRender instance;
			return &instance;
		}

		void DrawImage(TextureView texView, const ImVec2 &size = ImVec2(0, 0), const ImVec2 &uv0 = ImVec2(0, 0), const ImVec2 &uv1 = ImVec2(1, 1), const ImVec4 &tintColor = ImVec4(1, 1, 1, 1), const ImVec4 &borderColor = ImVec4(0, 0, 0, 0));

		bool DrawDropButton(std::vector<std::string> *output, bool isDebugLog = false);

		void NewFrame();
		void EndFrame();
		void Render(CommandList *cmdList);
		void Draw();
	private:
		void CreateDescriptorHeap();

		ImTextureID GetOrRegisterImGuiTexture(TextureView texView);

		ImGuiRender() = default;
		~ImGuiRender()
		{
			Uninit();
		}

		D3D12_GPU_DESCRIPTOR_HANDLE GetOrCreateSRV(Texture *texture);

		Renderer *renderer = nullptr;
		Device *device = nullptr;
		SwapChain *swapChain = nullptr;

		std::shared_ptr<Texture> fontTexture;
		std::shared_ptr<TextureView> fontTextureView;

		ComPtr<ID3D12DescriptorHeap> pHeap = nullptr;






		bool isInit = false;
		const UINT maxSRVCount_ = 1024;
		std::unordered_map<Texture *, SRVHandlePair> srvMap_;
		std::vector<PendingImage> pendingImages_;
		UINT descriptorSize = 0;
		UINT currentIndex = 1; // 0番はフォント用に予約
		std::unordered_map<UINT64, ImTextureID> handleMap; // src.ptr -> ImTextureID
		//ImGuiRender(const ImGuiRender &) = delete;
		//ImGuiRender &operator=(const ImGuiRender &) = delete;
	};
}