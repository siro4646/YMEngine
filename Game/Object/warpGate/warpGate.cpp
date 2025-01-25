#include "warpGate.h"
#include "gameFrameWork/components/sprite/sprite.h"

#include "utility/inputSystem/keyBoard/keyBoardInput.h"

#include "camera/camera.h"
#include "camera/cameraManager.h"

#include "gameFrameWork/gameObject/gameObjectManager.h"

#include "gameFrameWork/material/pbrMaterial.h"

#include "renderTexture/renderTexture.h"

#include "gameFrameWork/sceneRenderRegistrar/sceneRenderRegistrar.h"

#include "device/device.h"
#include "commandList/commandList.h"
#include "Renderer/renderer.h"

#include "buffer/buffer.h"
#include "bufferView/bufferView.h"

#include "shader/shader.h"
#include "sampler/sampler.h"
#include "rootSignature/rootSignature.h"
#include "pipelineState/pipelineState.h"
#include "descriptorSet/DescriptorSet.h"

#include "gameFrameWork/sceneRenderRegistrar/sceneRenderRegistrar.h"

#include "gameFrameWork/components/objLoader/objLoader.h"

namespace ym
{
	int WarpGate::createNum = 0;
	void WarpGate::Init()
	{
		name = "WarpGate";
		/*if (createNum == 0)
		else
		{
			name = "WarpGate1";
		}*/
		//localTransform.Scale = { 3.0f,0.25f,3.0f };
		localTransform.Scale *= 2;
		//localTransform.Rotation = { 0.0f,0.0f,0.0f };
		//localTransform.Position = { 0.0f,-0.5f,1.2f };

		localTransform.Position = { 0.0f,2.0f,2.0f };

		//material->Init();
		renderTexture = std::make_shared<RenderTexture>();
		renderTexture->Init();
		renderTexture->Create(1280, 720, true);		

		auto &cM = CameraManager::Instance();
		Transform cameraTransform;
		cameraTransform.Position = { 0.0f,30.0f,0.0f };
		//cameraTransform.Rotation = { 90.0f,0.0f,0.0f };
		cameraTransform.Scale = { 1.0f,1.0f,1.0f };

		cameraName = "warpCamera" + createNum;

		camera = cM.CreateCamera(cameraName);
		camera->SetUp(localTransform.GetUp());
		camera->SetEye(localTransform.Position);
		camera->SetTarget(localTransform.Position + localTransform.GetForward() * -100);
		camera->SetDistance(100);
		renderTexture->SetCamera(cameraName);

		CreateBuffer();
		CreatePSO();
		createNum++;
		//renderTexture->Draw();
		auto sceneRenderRegistrar = SceneRenderRegistrar::Instance();
		renderTexture->Draw(this);
		sceneRenderRegistrar->AddRenderObject(this);
		//renderTexture->Draw();

		const wchar_t *modelFile = L"asset/model/sphere_smooth.obj";

		//material->Init();

		int flag = 0;
		flag |= ModelSetting::InverseV;
		//flag |= ModelSetting::InverseU;
		flag |= ModelSetting::AdjustCenter;
		flag |= ModelSetting::AdjustScale;
		ImportSettings importSetting = // これ自体は自作の読み込み設定構造体
		{
			modelFile,
			flag
		};

		//auto objLoader = AddComponent<OBJLoader>();
		//objLoader->Load(importSetting);
	}

	void WarpGate::FixedUpdate()
	{
		Object::FixedUpdate();
		renderCount = 0;
		//localTransform.Position += localTransform.GetForward() * 0.05f;
	}

	void WarpGate::Update()
	{
		auto &input = KeyboardInput::GetInstance();
		UpdateMatrix();
		camera->SetUp(localTransform.GetUp());
		camera->SetEye(localTransform.Position + localTransform.GetForward() * 2);
		camera->SetTarget(localTransform.Position + localTransform.GetForward() * -100);
		camera->SetDistance(100);
		renderTexture->SetCamera(cameraName);
		camera->UpdateViewMatrix();
		camera->UpdateProjectionMatrix(1280/720,90,0.1f,1000.0f);
		camera->UpdateShaderBuffer();


		if (input.GetKey("Y"))
		{
			localTransform.Rotation.y -= 1.0f;
		}
		if (input.GetKey("U"))
		{
			localTransform.Rotation.y += 1.0f;
		}
		Object::Update();

		renderCount = 0;
	}

	void WarpGate::Draw()
	{
		if (renderCount <= 1)
		{
			//return;
			if (renderTexture->Draw(this))
			{
				ym::ConsoleLog("%s\n", name.c_str());
				ym::ConsoleLog("通常の描画\n");
				ym::ConsoleLog("%d\n", renderCount);
				auto cmdList = Renderer::Instance()->GetGraphicCommandList();
				cmdList->GetCommandList()->SetPipelineState(pS->GetPSO());
				auto ds = descriptorSet_;
				auto camera = CameraManager::Instance().GetCamera("mainCamera");
				ds->Reset();
				ds->SetPsSrv(0, renderTexture->GetRenderTargetTextureView(MultiRenderTargets::Color)->GetDescInfo().cpuHandle);
				ds->SetPsSampler(0, sampler_->GetDescInfo().cpuHandle);
				ds->SetVsCbv(0, constBufferView_->GetDescInfo().cpuHandle);
				ds->SetVsCbv(1, camera->GetDescriptorHandle());
				cmdList->SetGraphicsRootSignatureAndDescriptorSet(rS_.get(), ds.get());
				cmdList->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				cmdList->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_->GetView());
				cmdList->GetCommandList()->IASetIndexBuffer(&indexBufferView_->GetView());
				cmdList->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
				renderCount++;
				renderTexture->Draw(this);
			}
		}
		//テクスチャの設定
		Object::Draw();
		//シーンを描画

	}

	void WarpGate::Uninit()
	{
		vs_.reset();
		ps_.reset();
		rS_.reset();
		pS.reset();
		vertexBuffer_.reset();
		vertexBufferView_.reset();
		indexBuffer_.reset();
		indexBufferView_.reset();
		constantBuffer_.reset();
		constBufferView_.reset();
		sampler_.reset();		

		renderTexture->Uninit();		
		Object::Uninit();
	}

	std::shared_ptr<Object> WarpGate::Clone()
	{
		auto copy = std::make_shared<WarpGate>(*this); // コピーコンストラクタを使用
		// 子オブジェクトは深いコピーを行う
		for (const auto &child : _childs) {
			copy->_childs.push_back(child->Clone());
		}
		return copy;
	}

	void WarpGate::CreateBuffer()
	{
		auto device = Renderer::Instance()->GetDevice();
		auto cmdList = Renderer::Instance()->GetGraphicCommandList();

		//頂点バッファ
		{
			auto trans = localTransform;
			/*float sX = trans.Scale.x;
			float sY = trans.Scale.y;
			float sZ = trans.Scale.z;*/
			auto normal = trans.GetForward().ToXMFLOAT3();
			vertex_[0] = 
			{ 
				XMFLOAT3(-1.f, 1.f, 0.0f),
				normal,
				XMFLOAT2(0.0f, 0.0f),
				XMFLOAT3(1.f, 1.f,1.0f),//使わない
				XMFLOAT4(1.f, 1.f,1.0f,1.0f)
			};//左上

			vertex_[1] =
			{
				XMFLOAT3(1.f, 1.f, .0f),
				normal,
				XMFLOAT2(1.0f, 0.0f),
				XMFLOAT3(1.f, 1.f,1.0f),//使わない
				XMFLOAT4(1.f, 1.f,1.0f,1.0f)
			};//右上

			vertex_[2] =
			{
				XMFLOAT3(-1.f, -1.f,0.0f),
				normal,
				XMFLOAT2(0.0f, 1.0f),
				XMFLOAT3(1.f, 1.f,1.0f),//使わない
				XMFLOAT4(1.f, 1.f,1.0f,1.0f)
			};//左下

			vertex_[3] =
			{
				XMFLOAT3(1.f, -1.f, 0.0f),
				normal,
				XMFLOAT2(1.0f, 1.0f),
				XMFLOAT3(1.f, 1.f,1.0f),//使わない
				XMFLOAT4(1.f, 1.f,1.0f,1.0f)
			};//右下



			vertexBuffer_ = std::make_shared<Buffer>();
			vertexBuffer_->Init(device, sizeof(Vertex3D)*4, sizeof(Vertex3D), BufferUsage::VertexBuffer, false, false);
			vertexBufferView_ = std::make_shared<VertexBufferView>();
			vertexBufferView_->Init(device, vertexBuffer_.get());
			vertexBuffer_->UpdateBuffer(device, cmdList, &vertex_, sizeof(Vertex3D) * 4);
		}

		//インデックスバッファ
		{
			indexBuffer_ = std::make_shared<Buffer>();
			indexBuffer_->Init(device, sizeof(index_), sizeof(u32), BufferUsage::IndexBuffer, false, false);
			indexBufferView_ = std::make_shared<IndexBufferView>();
			indexBufferView_->Init(device,indexBuffer_.get());
			indexBuffer_->UpdateBuffer(device, cmdList, &index_, sizeof(index_));
		}

		//定数バッファ
		{
			constantBuffer_ = std::make_shared<Buffer>();
			constantBuffer_->Init(device, sizeof(*pMatrix_), sizeof(XMMATRIX), BufferUsage::ConstantBuffer, true, false);
			constBufferView_ = std::make_shared<ConstantBufferView>();
			constBufferView_->Init(device, constantBuffer_.get());
			pMatrix_ = (XMMATRIX *)constantBuffer_->Map();
			*pMatrix_ = XMMatrixIdentity();
		}
	}

	void WarpGate::CreatePSO()
	{
		auto device = Renderer::Instance()->GetDevice();
		vs_ = std::make_shared<Shader>();
		ps_ = std::make_shared<Shader>();

		vs_->Init(device, ShaderType::Vertex, "unlit");
		ps_->Init(device, ShaderType::Pixel, "unlit");

		rS_ = std::make_shared<RootSignature>();
		rS_->Init(device, vs_.get(), ps_.get(), nullptr, nullptr, nullptr);

		{
			D3D12_SAMPLER_DESC desc{};
			desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			sampler_ = std::make_shared<Sampler>();
			sampler_->Initialize(device, desc);
		}

		{
			ym::GraphicsPipelineStateDesc desc;
			desc = GraphicsPipelineState::GetDefaultDesc();
			desc.rasterizer.cullMode = D3D12_CULL_MODE_BACK;
			desc.pRootSignature = rS_.get();
			desc.pVS = vs_.get();
			desc.pPS = ps_.get();
			pS = std::make_shared<GraphicsPipelineState>();
			pS->Init(device, desc);
		}

		descriptorSet_ = std::make_shared<DescriptorSet>();

	}

	void WarpGate::UpdateMatrix()
	{
		*pMatrix_ = worldTransform.GetMatrix();
	}


}

