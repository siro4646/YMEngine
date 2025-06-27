#pragma once

#include "device/device.h"
#include "commandList/commandList.h"

#include "rootSignature/rootSignature.h"
#include "descriptorSet/descriptorSet.h"
#include "pipelineState/pipelineState.h"
#include "shader/shader.h"
#include "sampler/sampler.h"

//テスト用
#include "test/polygon.h"

#include "postProcess/postProcessManager.h"

//=============================================================================


namespace ym
{
	class Device;
	class CommandList;
	class Window;
	class Texture;
	class TextureView;
	class RenderTargetView;
	class DepthStencilView;

	class Buffer;
	class VertexBufferView;
	class IndexBufferView;
	struct Vertex2D;

	class RootSignature;
	class DescriptorSet;
	class GraphicsPipelineState;
	class Shader;

	class PostProcessMaterial;
	class MosaicMaterial;
	class GrayScaleMaterial;
	class BlurMaterial;
	class LightPassMaterial;
	class LumPassMaterial;
	class SSRMaterial;
	class SSAAMaterial;
	class FXAAMaterial;
	class ToonMapMaterial;
	class DoFMaterial;
	class BloomMaterial;

	enum MultiRenderTargets
	{
		Color,//カラー
		Normal,//法線
		HighLuminance,//輝度
		WorldPos,//ワールド座標
		Depth,//深度
		Max,//最大数
		//ダブルバッファの場合 0,1,2と3,4,5のペアで使う
	};


	//描画関係全部やってくれるクラスになる予定
	class Renderer
	{
	public:
		static Renderer *Instance()
		{
			static Renderer instance;
			return &instance;
		}
		Renderer(const Renderer &) = delete;
		Renderer &operator=(const Renderer &) = delete;



		bool Init(Window *win, u32 width, u32 height, ColorSpaceType csType = ColorSpaceType::Rec709);

		void Uninit();

		void Update();

		void Resize(u32 width, u32 height);

		CommandList *GetGraphicCommandList()
		{
			return pGraphicCommandList_.get();
		}
		CommandList *GetRenderTextureCommandList()
		{
			return pRenderTextureCommandList_.get();
		}
		CommandList *GetComputeCommandList()
		{
			return pComputeCommandList_.get();
		}
		CommandList *GetCopyCommandList()
		{
			return pCopyCommandList_.get();
		}

		Device *GetDevice()
		{
			return device_.get();
		}

		Texture *GetDummyTexture(DummyTex::Type type)
		{
			return dummyTextures_[type].get();
		}
		TextureView *GetDummyTextureView(DummyTex::Type type)
		{
			return dummyTextureViews_[type].get();
		}

		Texture *GetSceneRenderTexture(int index,MultiRenderTargets type)
		{

			return sceneRenderTextures[index * MultiRenderTargets::Max + type].get();
		}
		RenderTargetView *GetSceneRenderTargetView(int index, MultiRenderTargets type)
		{
			return sceneRenderTargetViews[index * MultiRenderTargets::Max + type].get();
		}
		TextureView *GetSceneRenderTargetTexView(int index, MultiRenderTargets type)
		{
			return sceneRenderTargetTexViews[index * MultiRenderTargets::Max + type].get();
		}
		D3D12_CPU_DESCRIPTOR_HANDLE GetDescHandle(int index, MultiRenderTargets type)
		{
			return sceneRenderTargetViews[index * MultiRenderTargets::Max + type]->GetDescInfo().cpuHandle;
		}


		Texture *GetDepthStencilTexture()
		{
			return depthStencilTexture_.get();
		}
		DepthStencilView *GetDepthStencilView()
		{
			return depthStencilView_.get();
		}
		TextureView *GetDepthStencilTexView()
		{
			return depthStencilTexView_.get();
		}


		void BeginFrame();
		void Draw();
		void EndFrame();

		void SetViewPort();


	private:
		Renderer() = default;
		~Renderer()
		{
			
			Uninit();

		}

		void CreateDummyTextures();

		void CreateRenderTargets(u32 width, u32 height);

		void CreateDepthStencil(u32 width, u32 height);

		void CreateSceneBuffers();

		void CreateSceneRootSignature();

		void CreateSceneDescriptorSet();

		void CreateScenePipelineState();

		void CreateSceneShaders();

		void CreateSceneSampler();



	private:
		static std::shared_ptr<Device> device_;//デバイス
		static std::shared_ptr<CommandList> pGraphicCommandList_;
		static std::shared_ptr<CommandList> pRenderTextureCommandList_;

		static std::shared_ptr<CommandList> pComputeCommandList_;
		static std::shared_ptr<CommandList> pCopyCommandList_;

		ym::Window *window_{};
		D3D12_VIEWPORT m_viewPort{};
		D3D12_RECT m_scissorRect{};
		bool isUninited = false;

		std::vector<std::unique_ptr<Texture>>		dummyTextures_;
		std::vector<std::unique_ptr<TextureView>>	dummyTextureViews_;

		//レンダーターゲット
		std::vector<std::shared_ptr<Texture>> sceneRenderTextures;
		std::vector<std::shared_ptr<RenderTargetView>> sceneRenderTargetViews;
		std::vector<std::shared_ptr<TextureView>> sceneRenderTargetTexViews;
		//デプスステンシル
		std::shared_ptr<Texture> depthStencilTexture_;
		std::shared_ptr<DepthStencilView> depthStencilView_;
		std::shared_ptr<TextureView> depthStencilTexView_;

		//シーン描画用
		std::shared_ptr<Buffer> sceneVertexBuffer_;
		std::shared_ptr<VertexBufferView> sceneVertexBufferView_;
		Vertex2D sceneVertices_[4]
		{
			{XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f)},
			{XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)},
			{XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f)},
			{XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f)}
		};
		std::shared_ptr<Buffer> sceneIndexBuffer_;
		std::shared_ptr<IndexBufferView> sceneIndexBufferView_;
		std::vector<u32> sceneIndices_
		{
			0,1,2,1,3,2
		};

		std::shared_ptr<MosaicMaterial> mosaicMaterial_;
		std::shared_ptr<GrayScaleMaterial> grayScaleMaterial_;
		float blurValue_ = 0.0f;
		std::shared_ptr<LightPassMaterial> lightPassMaterial_;
		std::shared_ptr<LumPassMaterial> lumPassMaterial_;
		std::shared_ptr<SSRMaterial> ssrMaterial_;
		std::shared_ptr<SSAAMaterial> ssaaMaterial_;
		std::shared_ptr<FXAAMaterial> fxaaMaterial_;
		std::shared_ptr<ToonMapMaterial> toonMapMaterial_;
		std::shared_ptr<DoFMaterial> dofMaterial_;
		std::shared_ptr<BloomMaterial> bloomMaterial_;

		bool usePostProcess_[(int)PostProcessType::Max]{ false };

		std::shared_ptr<RootSignature> sceneRootSignature_;
		std::shared_ptr<DescriptorSet> sceneDescriptorSet_;
		std::shared_ptr<GraphicsPipelineState> scenePipelineState_;
		std::shared_ptr<Shader> sceneVS_;
		std::shared_ptr<Shader> scenePS_;
		std::shared_ptr<Sampler> sceneSampler_;
	};


}