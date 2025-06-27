#pragma once

#include "gameFrameWork/components/component.h"

#include "Renderer/renderer.h"

namespace ym
{

	class Texture;
	class RenderTargetView;
	class TextureView;
	class DepthStencilView;
	class Camera;

	class RenderTexture
	{
	public:
		void Init();
		void FixedUpdate();
		void Update();
		bool Draw(Object*);
		bool Draw();
		void Uninit();
		void Create(u32 width, u32 height, bool createDepthStencil = false);
		void SetCamera(std::string useCameraName) {
			useCameraName_ = useCameraName;
		}

		std::shared_ptr<Texture> GetRenderTargetTexture(u32 index = 0) {

			auto next = ((bbidx + 1) % SwapChain::kFrameCount) * MultiRenderTargets::Max + index;
			return renderTargetTexture_[next];
		}

		std::shared_ptr<RenderTargetView> GetRenderTargetView(u32 index = 0) {
			auto next = ((bbidx + 1) % SwapChain::kFrameCount) * MultiRenderTargets::Max + index;

			return renderTargetView_[next];
		}

		std::shared_ptr<TextureView> GetRenderTargetTextureView(u32 index = 0) {
			int next = ((bbidx + 1) % SwapChain::kFrameCount) * MultiRenderTargets::Max + index;			
			return renderTargetTextureView_[next];
		}


	private:
		
		void CreateRenderTargets(u32 width, u32 height);

		void CreateDepthStencil(u32 width, u32 height);
		int bbidx = 0;
		//int 
		bool createDepthStencil_ = true;
		bool isCreated_ = false;
		std::vector<std::shared_ptr<Texture>> renderTargetTexture_;
		std::vector<std::shared_ptr<RenderTargetView>> renderTargetView_;
		std::vector<std::shared_ptr<TextureView>> renderTargetTextureView_;
		std::shared_ptr<DepthStencilView> depthStencilView_;
		std::shared_ptr<Texture> depthStencilTexture_;
		std::string useCameraName_{};
	};
}