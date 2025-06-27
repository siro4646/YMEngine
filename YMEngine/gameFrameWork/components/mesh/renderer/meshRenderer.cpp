#include "meshRenderer.h"

#include "../instance/meshInstance.h"
#include "../filter/meshFilter.h"

#include "Renderer/renderer.h"
#include "device/device.h"
#include "commandList/commandList.h"

//===========================================================
#include "camera/camera.h"
#include "camera/cameraManager.h"

#include "sampler/sampler.h"
#include "rootSignature/rootSignature.h"
#include "pipelineState/pipelineState.h"
#include "shader/shader.h"

#include "material/material2.h"

//===========================================================


namespace ym
{
	namespace mesh
	{
		void MeshRenderer::Init()
		{
			// 初期化時の処理（必要に応じて）
			renderer_ = Renderer::Instance();
			if (renderer_)
			{
				device_ = renderer_->GetDevice();
				cmdList_ = renderer_->GetGraphicCommandList();
			}
			// MeshFilterの取得
			meshFilter_ = GetComponent<MeshFilter>().get();

			descriptorSet_ = std::make_shared<DescriptorSet>();
			BaseShader();
			BaseRootSignature();
			BasePipelineState();
			BaseSampler();
			//auto mat = new ym::material::Material();
			//mat->Init("pbrVS", "pbrPS");
			//mat->SetTexture("g_texture", "aaa");
			//mat->SetTexture("g_maskTexture", "white");
			////material_->Init(); // シェーダーの初期化
			//mat->SetTexture("g_texture","aaa"); // テクスチャの設定
			//mat->SetTexture("g_maskTexture", "white"); // マスクテクスチャの設定
			//mat->SetMatrix("mat", object->worldTransform.GetMatrix());
			//mat->SetMatrix("view", CameraManager::Instance().GetMainCamera()->GetViewMatrix());
			//mat->SetMatrix("proj", CameraManager::Instance().GetMainCamera()->GetProjectionMatrix());
			//mat->SetMatrix("invView", CameraManager::Instance().GetMainCamera()->GetInvViewMatrix()); // 逆ビュー行列
			//mat->SetMatrix("invProj", CameraManager::Instance().GetMainCamera()->GetInvProjectionMatrix()); // 逆プロジェクション行列
			//mat->SetMatrix("invViewProj", CameraManager::Instance().GetMainCamera()->GetInvViewProjectionMatrix());
			//materials_.push_back(mat);
			//material_->SetFloat("eye", CameraManager::Instance().GetMainCamera()->GetEye());
		}

		void MeshRenderer::Update()
		{
			//material_->SetMatrix("mat", object->worldTransform.GetMatrix());
			//material_->SetMatrix("view", CameraManager::Instance().GetMainCamera()->GetViewMatrix());
			//material_->SetMatrix("proj", CameraManager::Instance().GetMainCamera()->GetProjectionMatrix());
			//material_->SetMatrix("invView", CameraManager::Instance().GetMainCamera()->GetInvViewMatrix()); // 逆ビュー行列
			//material_->SetMatrix("invProj", CameraManager::Instance().GetMainCamera()->GetInvProjectionMatrix()); // 逆プロジェクション行列
			//material_->SetMatrix("invViewProj", CameraManager::Instance().GetMainCamera()->GetInvViewProjectionMatrix());
			//material_->SetFloat3("eye", CameraManager::Instance().GetMainCamera()->GetEye());
			//material_->SetFloat("nearZ", CameraManager::Instance().GetMainCamera()->GetNearZ());
			//material_->SetFloat("farZ", CameraManager::Instance().GetMainCamera()->GetFarZ());
			//material_->SetFloat("aspect", CameraManager::Instance().GetMainCamera()->GetAspectRatio());
			//material_->SetFloat("fov", CameraManager::Instance().GetMainCamera()->GetFov());
			////material_->SetFloat3("color", DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f)); // デフォルトの色
			//material_->Update();
			if (!meshFilter_) return;
			for (auto &mat : materials_)
			{
				mat->SetMatrix("mat", object->worldTransform.GetMatrix());
				mat->SetMatrix("view", CameraManager::Instance().GetMainCamera()->GetViewMatrix());
				mat->SetMatrix("proj", CameraManager::Instance().GetMainCamera()->GetProjectionMatrix());
				mat->SetMatrix("invView", CameraManager::Instance().GetMainCamera()->GetInvViewMatrix()); // 逆ビュー行列
				mat->SetMatrix("invProj", CameraManager::Instance().GetMainCamera()->GetInvProjectionMatrix()); // 逆プロジェクション行列
				mat->SetMatrix("invViewProj", CameraManager::Instance().GetMainCamera()->GetInvViewProjectionMatrix());
				mat->SetFloat3("eye", CameraManager::Instance().GetMainCamera()->GetEye());
				mat->Update();
			}
		}

		//void MeshRenderer::Draw()
		//{
		//	if (!meshFilter_)
		//	{
		//		return; // MeshFilterが設定されていない場合は何もしない
		//	}
		//	// 描画処理をここに実装
		//	//マテリアル担当部分
		//	auto cmdList = cmdList_->GetCommandList();
		//	//cmdList->SetPipelineState(pipelineState_->GetPSO());
		//	auto ds = descriptorSet_.get();
		//	//ds->Reset();
		//	material_->ApplyConstantBuffer(ds);
		//	//cmdList_->SetGraphicsRootSignatureAndDescriptorSet(rootSignature_.get(), descriptorSet_.get());
		//	meshFilter_->ApplyBuffers();
		//	cmdList->DrawIndexedInstanced(meshFilter_->GetMeshInstance()->GetIndices().size(), 1, 0, 0, 0);
		//}
		void MeshRenderer::Draw()
		{
			if (!meshFilter_) return;

			const auto &mesh = meshFilter_->GetMeshInstance()->GetMesh();
			const auto &subs = mesh.SubMeshes;

			auto cmd = cmdList_->GetCommandList();
			meshFilter_->ApplyBuffers();                // ★ VBO / IBO を 1回だけセット

			for (size_t i = 0; i < subs.size(); ++i)
			{
				const auto &sm = subs[i];

				// マテリアル取得（足りなければデフォルト）
				ym::material::Material *mat =
					(i < materials_.size()) ? materials_[i].get() : nullptr;

				if (mat) {
					mat->ApplyConstantBuffer(descriptorSet_.get());
				}

				cmd->DrawIndexedInstanced(
					sm.indexCount,        // IndexCountPerInstance
					1,                    // InstanceCount
					sm.indexOffset,       // StartIndexLocation
					0,                    // BaseVertexLocation
					0);                   // StartInstanceLocation
			}
		}



		void MeshRenderer::Uninit()
		{
			meshFilter_ = nullptr;
			renderer_ = nullptr;
			device_ = nullptr;
			cmdList_ = nullptr;
			vs_.reset();
			ps_.reset();
			rootSignature_.reset();
			pipelineState_.reset();
			sampler_.reset();
			descriptorSet_.reset();
		}

		void MeshRenderer::DrawImguiBody()
		{
			for (size_t i = 0; i < materials_.size(); ++i)
			{
				ImGui::PushID(static_cast<int>(i));
				if (materials_[i])
				{
					ImGui::Text("Material [%zu]", i);
					materials_[i]->DrawImGui();
				}
				ImGui::PopID();
			}

			//material_->DrawImGui();	
		}
		
		void MeshRenderer::BaseShader()
		{
			vs_ = std::make_shared<Shader>();
			vs_->Init(device_, ShaderType::Vertex, "unlit");
			ps_ = std::make_shared<Shader>();
			ps_->Init(device_, ShaderType::Pixel, "unlit");
		}
		void MeshRenderer::BasePipelineState()
		{
			auto bbidx = device_->GetSwapChain().GetFrameIndex();
			auto &swapChain = device_->GetSwapChain();

			ym::GraphicsPipelineStateDesc desc;
			desc = GraphicsPipelineState::GetDefaultDesc();
			desc.rasterizer.cullMode = D3D12_CULL_MODE_NONE;
			desc.pRootSignature = rootSignature_.get();
			desc.pVS = vs_.get();
			desc.pPS = ps_.get();
			pipelineState_ = std::make_shared<GraphicsPipelineState>();
			pipelineState_->Init(device_, desc);
		}
		void MeshRenderer::BaseRootSignature()
		{
			rootSignature_ = std::make_shared<RootSignature>();
			rootSignature_->Init(device_, vs_.get(), ps_.get(),nullptr, nullptr, nullptr);
		}
		void MeshRenderer::BaseSampler()
		{
			D3D12_SAMPLER_DESC desc{};
			desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			sampler_ = std::make_shared<Sampler>();
			sampler_->Initialize(device_, desc);
		}
	}
}