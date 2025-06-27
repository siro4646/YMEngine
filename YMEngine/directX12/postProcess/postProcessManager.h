#pragma once
#include "swapChain/swapChain.h"

namespace ym
{

	enum  class  PostProcessType :u32
	{
		Blur,
		Mosaic,
		GrayScale,
		Max,
	};

	class PostProcessMaterial;

	class Device;
	class CommandList;

	class Buffer;
	class VertexBufferView;
	class IndexBufferView;
	struct Vertex2D;

	class Texture;
	class TextureView;
	class RenderTargetView;


	class PostProcessManager
	{
	public:
		~PostProcessManager() {
			Uninit();
		}

		static PostProcessManager *Instance()
		{
			/*static PostProcessManager*instance = nullptr;
			if (instance == nullptr)
			{
				instance = new PostProcessManager();
			}
			return instance;*/

			static PostProcessManager instance;
			return &instance;

		}


		void Init();
		void Uninit();
		void Update();
		void Draw();

		void AddPostProcessMaterial(PostProcessMaterial *material);
		void RemovePostProcessMaterial(PostProcessMaterial *material);

		TextureView *GetResultTextureView(u32 idx)
		{
			return resultTextureViews_[idx%SwapChain::kFrameCount];
		}
		Texture *GetResultTexture(u32 idx)
		{
			return resultTextures_[idx % SwapChain::kFrameCount];
		}
		RenderTargetView *GetResultRTV(u32 idx)
		{
			return resultRTVs_[idx % SwapChain::kFrameCount];
		}

	private:

		void CreateVertexBuffer();
		void CreateIndexBuffer();


		PostProcessManager() {}
		Device *device_;
		CommandList *graphicsCmdList_;

		std::vector<PostProcessMaterial*> materials_;

		std::vector<Texture*> resultTextures_;
		std::vector<TextureView*> resultTextureViews_;
		std::vector<RenderTargetView*> resultRTVs_;

		std::shared_ptr<Buffer> vertexBuffer_;
		std::shared_ptr<VertexBufferView> vertexBufferView_;
		Vertex2D vertices_[4]
		{
			{DirectX::XMFLOAT3(-1.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f)},
			{DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f)},
			{DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f)},
			{DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f)}
		};
		std::shared_ptr<Buffer> indexBuffer_;
		std::shared_ptr<IndexBufferView> indexBufferView_;
		u32 indices_[6]
		{
			0, 1, 2, 0, 2, 3
		};
	};
}
