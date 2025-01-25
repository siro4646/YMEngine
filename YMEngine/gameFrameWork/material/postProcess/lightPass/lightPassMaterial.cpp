#include "lightPassMaterial.h"
#include "device/device.h"
#include "commandList/commandList.h"
#include "pipelineState/pipelineState.h"
#include "descriptorSet/DescriptorSet.h"
#include "shader/shader.h"
#include "sampler/sampler.h"

#include "postProcess/postProcessManager.h"
#include "Renderer/renderer.h"

#include "buffer/buffer.h"
#include "bufferView/bufferView.h"

#include "camera/cameraManager.h"
#include "camera/camera.h"

#include "gameFrameWork/light/lightManager.h"

namespace ym
{
	void LightPassMaterial::Init()
	{
		PostProcessMaterial::Init();

		CreateShader();
		PostProcessMaterial::BaseRootSignature();
		PostProcessMaterial::BaseSampler();
		PostProcessMaterial::BasePipelineState();
		CreateConstantBuffer();

		//CreatePipelineState();
	}
	void LightPassMaterial::Uninit()
	{
		PostProcessMaterial::Uninit();



		constantBuffer_->Unmap();
		constantBuffer_.reset();
		constantBufferView_->Destroy();
		constantBufferView_.reset();


	}
	void LightPassMaterial::Update()
	{
		PostProcessMaterial::Update();
		UpdateBuffer();
		
	}
	void LightPassMaterial::Draw()
	{
		
		ImGui::Begin("LightPassMaterial");
		//float dir[3]
		ImGui::SliderFloat3("Direction", dir, -1.0f, 1.0f);
		ImGui::SliderFloat3("Color", color, 0.0f, 1.0f);
		ImGui::SliderFloat("Intensity", &intensity, 0.0f, 5.0f);
		ImGui::End();

		light_.direction = XMFLOAT3(dir[0], dir[1], dir[2]);
		light_.color = XMFLOAT3(color[0], color[1], color[2]);
		light_.intensity = intensity;



		//auto tex = PostProcessManager::Instance()->GetPostProcessTexture();		
		//auto scTex = device_->GetSwapChain().GetCurrentTexture();

		/*descriptorSet_->Reset();
		descriptorSet_->SetPsSrv()*/

		//PostProcessMaterial::Draw();
	}
	void LightPassMaterial::SetMaterial()
	{
		PostProcessMaterial::SetMaterial();
		auto bbidx = device_->GetSwapChain().GetFrameIndex();
		auto cmdList = graphicsCmdList_;
		auto renderer = Renderer::Instance();
		auto colorTex = renderer->GetSceneRenderTexture(bbidx, MultiRenderTargets::Color);
		auto colorView = renderer->GetSceneRenderTargetTexView(bbidx, MultiRenderTargets::Color);
		auto normalTex = renderer->GetSceneRenderTexture(bbidx, MultiRenderTargets::Normal);
		auto normalView = renderer->GetSceneRenderTargetTexView(bbidx, MultiRenderTargets::Normal);
		auto worldPosTex = renderer->GetSceneRenderTexture(bbidx, MultiRenderTargets::WorldPos);
		auto worldPosView = renderer->GetSceneRenderTargetTexView(bbidx, MultiRenderTargets::WorldPos);

		auto camera = CameraManager::Instance().GetCamera("mainCamera");

		auto lightManager = LightManager::Instance();

		//テクスチャをシェーダーに渡すためにバリアを遷移
		cmdList->TransitionBarrier(colorTex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		cmdList->TransitionBarrier(normalTex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		cmdList->TransitionBarrier(worldPosTex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		cmdList->GetCommandList()->SetPipelineState(pipelineState_->GetPSO());
		descriptorSet_->Reset();
		descriptorSet_->SetPsSrv(0, colorView->GetDescInfo().cpuHandle);
		descriptorSet_->SetPsSrv(1, normalView->GetDescInfo().cpuHandle);
		descriptorSet_->SetPsSrv(2, worldPosView->GetDescInfo().cpuHandle);	
		descriptorSet_->SetPsCbv(0, camera->GetDescriptorHandle());
		descriptorSet_->SetPsCbv(1, constantBufferView_->GetDescInfo().cpuHandle);
		descriptorSet_->SetPsSrv(3, lightManager->GetPointLightCPUHandle());
		descriptorSet_->SetPsCbv(2, lightManager->GetConstantBufferView()->GetDescInfo().cpuHandle);
		descriptorSet_->SetPsSampler(0, sampler_->GetDescInfo().cpuHandle);
		cmdList->SetGraphicsRootSignatureAndDescriptorSet(rootSignature_.get(), descriptorSet_.get());

	}
	void LightPassMaterial::CreateShader()
	{
		vs_->Init(device_, ShaderType::Vertex, "lightPass");
		ps_->Init(device_, ShaderType::Pixel, "lightPass");
	}
	void LightPassMaterial::CreatePipelineState()
	{
		auto bbidx = device_->GetSwapChain().GetFrameIndex();

		D3D12_INPUT_ELEMENT_DESC elementDescs[] = {
{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},

		};
		ym::GraphicsPipelineStateDesc desc;
		desc.rasterizer.fillMode = D3D12_FILL_MODE_SOLID;
		desc.rasterizer.cullMode = D3D12_CULL_MODE_NONE;
		desc.rasterizer.isFrontCCW = false;
		desc.rasterizer.isDepthClipEnable = false;
		desc.multisampleCount = 1;

		desc.blend.sampleMask = UINT_MAX;
		desc.blend.rtDesc[0].isBlendEnable = false;
		desc.blend.rtDesc[0].srcBlendColor = D3D12_BLEND_ONE;
		desc.blend.rtDesc[0].dstBlendColor = D3D12_BLEND_ZERO;
		desc.blend.rtDesc[0].blendOpColor = D3D12_BLEND_OP_ADD;
		desc.blend.rtDesc[0].srcBlendAlpha = D3D12_BLEND_ONE;
		desc.blend.rtDesc[0].dstBlendAlpha = D3D12_BLEND_ZERO;
		desc.blend.rtDesc[0].blendOpAlpha = D3D12_BLEND_OP_ADD;
		desc.blend.rtDesc[0].writeMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		desc.depthStencil.isDepthEnable = false;
		desc.depthStencil.isDepthWriteEnable = false;

		desc.pRootSignature = rootSignature_.get();
		desc.pVS = vs_.get();
		desc.pPS = ps_.get();
		desc.primTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		desc.inputLayout.numElements = _countof(elementDescs);
		desc.inputLayout.pElements = elementDescs;
		desc.numRTVs = 0;
		desc.rtvFormats[desc.numRTVs++] = device_->GetSwapChain().GetRenderTargetView(bbidx)->GetFormat();

		pipelineState_->Init(device_, desc);

	}
	void LightPassMaterial::UpdateBuffer()
	{
		void *p = (DirectinalLight*)constantBuffer_->Map();
		if (p)
		{
			memcpy(p, &light_, sizeof(light_));
			constantBuffer_->Unmap();
		}

		//pLight_ = &light_;
	}
	void LightPassMaterial::CreateConstantBuffer()
	{
		auto renderer = Renderer::Instance();
		device_ = renderer->GetDevice();
		graphicsCmdList_ = renderer->GetGraphicCommandList();

		constantBuffer_ = std::make_shared<Buffer>();
		constantBuffer_->Init(device_, sizeof(*pLight_), sizeof(DirectinalLight), BufferUsage::ConstantBuffer , true, false);
		constantBufferView_ = std::make_shared<ConstantBufferView>();
		constantBufferView_->Init(device_, constantBuffer_.get());
		pLight_ = (DirectinalLight*)constantBuffer_->Map();

		dir[0] = 0.0f;
		dir[1] = 1.0f;
		dir[2] = -1.0f;


		color[0] = 1.0f;
		color[1] = 1.0f;
		color[2] = 1.0f;

		intensity = 5.0f;
		light_.direction = XMFLOAT3(dir[0], dir[1], dir[2]);
		light_.color = XMFLOAT3(color[0], color[1], color[2]);
		light_.intensity = intensity;
		light_.padding = 0.0f;

		UpdateBuffer();

	}
}
