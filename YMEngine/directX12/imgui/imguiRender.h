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

		void NewFrame();
		void EndFrame();
		void Render(CommandList *cmdList);
		void Draw();

	private:
		ImGuiRender() = default;
		~ImGuiRender()
		{
			Uninit();
		}
		Renderer *renderer = nullptr;
		Device *device = nullptr;
		SwapChain *swapChain = nullptr;

		std::shared_ptr<Texture> fontTexture;
		std::shared_ptr<TextureView> fontTextureView;

		ComPtr<ID3D12DescriptorHeap> pHeap = nullptr;





		bool isInit = false;




		//ImGuiRender(const ImGuiRender &) = delete;
		//ImGuiRender &operator=(const ImGuiRender &) = delete;
	};
}