#pragma once

#include "textureView/textureView.h"

namespace ym
{
	class RenderTargetView;
	class Texture;
	class DepthStencilView;

	class CommandList;

	class RenderTargetManager
	{
	public:
		static RenderTargetManager *Instance()
		{
			static RenderTargetManager instance;
			return &instance;
		}
		RenderTargetManager() = default;
		~RenderTargetManager()
		{
			Uninit();
		}

		void Init();
		void Uninit();

		void BeginRenderTarget(std::vector<RenderTargetView>rtvs,std::vector<Texture>rts,DepthStencilView*dsv,Texture*dst,bool clearFlag = true);

		void PushRenderTarget(std::vector<RenderTargetView>rtvs, std::vector<Texture>rts, DepthStencilView *dsv,Texture *dst, bool clearFlag = true);

		void PopRenderTarget();

	private:
		CommandList *cmdList_{ nullptr };
		std::vector< RenderTargetView> prvRtvs_;
		DepthStencilView *prvDsv_{ nullptr };
		Texture *prvDst_{ nullptr };
	};
}