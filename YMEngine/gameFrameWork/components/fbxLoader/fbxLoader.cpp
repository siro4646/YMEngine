#include "fbxLoader.h"

#include "device/device.h"
#include "commandList/commandList.h"

#include "gameFrameWork/gameObject/gameObject.h"

#include "resource/resourceStateTracker.h"
#include "shader/shader.h"

#include "buffer/buffer.h"
#include "bufferView/bufferView.h"
#include "rootSignature/rootSignature.h"
#include "pipelineState/pipelineState.h"
#include "descriptorSet/DescriptorSet.h"
#include "sampler/sampler.h"


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "texture/texture.h"
#include "textureView/textureView.h"

#include "camera/camera.h"
#include "camera/cameraManager.h"

#include "utility/inputSystem/keyBoard/keyBoardInput.h"





namespace ym
{
	//const D3D12_INPUT_ELEMENT_DESC FBXLoader::Vertex::InputElements[] =
	//{
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // float3のPOSITION
	//	//{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // float3のNORMAL
	//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // float2のTEXCOORD
	//	//{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // float3のTANGENT
	//	{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // float4のCOLOR
	//};
	//const D3D12_INPUT_LAYOUT_DESC FBXLoader::Vertex::InputLayout =
	//{
	//	Vertex::InputElements,
	//	_countof(Vertex::InputElements)
	//};

	void FBXLoader::Init()
	{
		CreatePipelineState();
		CreateSampler();
		CreateConstantBuffer();
		descriptorSet_ = std::make_shared<DescriptorSet>();

		pCamera_ = CameraManager::Instance().GetMainCamera();
		if (!pCamera_)
		{
			ym::ConsoleLog("カメラが設定されていません\n");
		}

	}
	void FBXLoader::FixedUpdate()
	{
	}
	void FBXLoader::Update()
	{
		auto &input = KeyboardInput::GetInstance();
		if (input.GetKeyDown("G"))
		{
			//座標を表示
			ym::ConsoleLog("x:%f y:%f z:%f\n", object->globalTransform.Position.x, object->globalTransform.Position.y, object->globalTransform.Position.z);
		}

		UpdateMatrix();
	}
	void FBXLoader::Draw()
	{
		if (!isInit_) return;
		auto cmdList = pCmdList_->GetCommandList();
		
		for (int i = 0; i < meshes.size(); i++)
		{
		cmdList->SetPipelineState(pipelineState_->GetPSO());
		descriptorSet_->Reset();
		descriptorSet_->SetVsCbv(0, constBufferView_->GetDescInfo().cpuHandle);
		descriptorSet_->SetVsCbv(1, pCamera_->GetDescriptorHandle());
		descriptorSet_->SetPsSrv(0, textureViews_[i]->GetDescInfo().cpuHandle);
		descriptorSet_->SetPsSampler(0, sampler_->GetDescInfo().cpuHandle);
		pCmdList_->SetGraphicsRootSignatureAndDescriptorSet(rootSignature_.get(), descriptorSet_.get());
			cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			cmdList->IASetVertexBuffers(0, 1, &vertexBufferViews_[i]->GetView());
			cmdList->IASetIndexBuffer(&indexBufferViews_[i]->GetView());
			cmdList->DrawIndexedInstanced(meshes[i].Indices.size(), 1, 0, 0, 0);
		}

	}
	void FBXLoader::Uninit()
	{
	}
	bool FBXLoader::Load(ImportSettings settings)
	{
		//
		if (settings.filename == nullptr)
		{
			return false;
		}
		//auto &meshes = settings.meshes;
		// フラグを取得
		flags_ = settings.flags;
		bool inverseU = false;
		bool inverseV = false;
		if (flags_ & ModelSetting::InverseU)
		{
			inverseU = true;
			
		}
		if (flags_ & ModelSetting::InverseV)
		{
			inverseV = true;
		}
		auto path = ym::Utf16ToUtf8(settings.filename);
		Assimp::Importer importer;
		int flag = 0;
		flag |= aiProcess_Triangulate;
		flag |= aiProcess_PreTransformVertices;
		flag |= aiProcess_CalcTangentSpace;
		flag |= aiProcess_GenSmoothNormals;
		flag |= aiProcess_GenUVCoords;
		flag |= aiProcess_RemoveRedundantMaterials;
		flag |= aiProcess_OptimizeMeshes;

		auto scene = importer.ReadFile(path, flag);

		if (scene == nullptr)
		{
			// もし読み込みエラーがでたら表示する
			printf(importer.GetErrorString());
			printf("\n");
			return false;
		}

		// 読み込んだデータを自分で定義したMesh構造体に変換する
		meshes.clear();
		meshes.resize(scene->mNumMeshes);
		texturePaths_.resize(scene->mNumMeshes);
		textures_.resize(scene->mNumMeshes);
		textureViews_.resize(scene->mNumMeshes);
		for (size_t i = 0; i < meshes.size(); ++i)
		{
			const auto pMesh = scene->mMeshes[i];
			LoadMesh(meshes[i], pMesh, inverseU, inverseV);
			const auto pMaterial = scene->mMaterials[i];
			LoadTexture(settings.filename, meshes[i], pMaterial);
			texturePaths_[i] = ym::Utf16ToUtf8(meshes[i].DiffuseMap);
			ym::ConsoleLog("texturePath:%s\n", texturePaths_[i].c_str());
			textures_[i] = std::make_shared<Texture>();
			textures_[i]->LoadTexture(pDevice_.get(), pCmdList_.get(), texturePaths_[i]);
			textureViews_[i] = std::make_shared<TextureView>();
			textureViews_[i]->Init(pDevice_.get(), textures_[i].get());
		}

		NormalizeScale();

		scene = nullptr;

		CreateVertexBuffer();
		CreateIndexBuffer();

		isInit_ = true;


		/*pCmdList_->Close();
		pCmdList_->Execute();
		pDevice_->WaitForCommandQueue();
		pCmdList_->Reset();	*/	

		return true;


	}
	void FBXLoader::LoadMesh(Mesh &dst, const aiMesh *src, bool inverseU, bool inverseV)
	{
		aiVector3D zero3D(0.0f, 0.0f, 0.0f);
		aiColor4D zeroColor(0.0f, 0.0f, 0.0f, 0.0f);

		dst.Vertices.resize(src->mNumVertices);

		bool isCalc = false;
		if (flags_ & AdjustScale || flags_ & AdjustCenter)
		{
			isCalc = true;
		}
		

		for (auto i = 0u; i < src->mNumVertices; ++i)
		{
			auto position = &(src->mVertices[i]);
			auto normal = &(src->mNormals[i]);
			auto uv = (src->HasTextureCoords(0)) ? &(src->mTextureCoords[0][i]) : &zero3D;
			auto tangent = (src->HasTangentsAndBitangents()) ? &(src->mTangents[i]) : &zero3D;
			auto color = (src->HasVertexColors(0)) ? &(src->mColors[0][i]) : &zeroColor;


			// 反転オプションがあったらUVを反転させる
			if (inverseU)
			{
				uv->x = 1 - uv->x;
			}
			if (inverseV)
			{
				uv->y = 1 - uv->y;
			}

			Vertex3D vertex = {};
			vertex.Position = DirectX::XMFLOAT3(position->x, position->y, position->z);
			vertex.Normal = DirectX::XMFLOAT3(normal->x, normal->y, normal->z);
			vertex.UV = DirectX::XMFLOAT2(uv->x, uv->y);
			vertex.Tangent = DirectX::XMFLOAT3(tangent->x, tangent->y, tangent->z);
			vertex.Color = DirectX::XMFLOAT4(color->r, color->g, color->b, color->a);
			if (isCalc) {
				minX = std::min(minX, vertex.Position.x);
				minY = std::min(minY, vertex.Position.y);
				minZ = std::min(minZ, vertex.Position.z);

				maxX = std::max(maxX, vertex.Position.x);
				maxY = std::max(maxY, vertex.Position.y);
				maxZ = std::max(maxZ, vertex.Position.z);
			}

			dst.Vertices[i] = vertex;
		}
			
		dst.Indices.resize(src->mNumFaces * 3);

		for (auto i = 0u; i < src->mNumFaces; ++i)
		{
			const auto &face = src->mFaces[i];

			dst.Indices[i * 3 + 0] = face.mIndices[0];
			dst.Indices[i * 3 + 1] = face.mIndices[1];
			dst.Indices[i * 3 + 2] = face.mIndices[2];
		}
	}
	void FBXLoader::NormalizeScale()
	{
		bool isCalcSize = false;
		bool isCalcCenter = false;
		float centerX;
		float centerY;
		float centerZ;
		float sizeX;
		float sizeY;
		float sizeZ;
		float maxDimension;
		float targetSize;
		float scaleFactor;
		if (flags_ & AdjustCenter)
		{
			centerX = (minX + maxX) / 2.0f;
			centerY = (minY + maxY) / 2.0f;
			centerZ = (minZ + maxZ) / 2.0f;
			isCalcCenter = true;
		}
		if (flags_ & AdjustScale)
		{
			sizeX = maxX - minX;
			sizeY = maxY - minY;
			sizeZ = maxZ - minZ;
			maxDimension = std::max({ sizeX, sizeY, sizeZ });
			targetSize = 1.0f; // 目標の大きさ
			scaleFactor = targetSize / maxDimension;// 目標の大きさに合わせるためのスケール
			isCalcSize = true;
		}
		if (!isCalcSize && !isCalcCenter)
		{
			return;
		}
		// スケールを適用
		for (int i = 0; i < meshes.size(); i++)
		{
			for (auto &vertex : meshes[i].Vertices)
			{
				if (isCalcCenter)
				{
					vertex.Position.x -= centerX;
					vertex.Position.y -= centerY;
					vertex.Position.z -= centerZ;
				}
				if (isCalcSize)
				{
					vertex.Position.x *= scaleFactor;
					vertex.Position.y *= scaleFactor;
					vertex.Position.z *= scaleFactor;
				}
			}
		}


	}
	void FBXLoader::LoadTexture(const wchar_t *filename,Mesh &dst, const aiMaterial *src)
	{

		aiString path;
		if (src->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == AI_SUCCESS)
		{
			// テクスチャパスは相対パスで入っているので、ファイルの場所とくっつける
			auto dir = GetDirectoryPath(filename);
			auto file = std::string(path.C_Str());
			dst.DiffuseMap = dir + ym::Utf8ToUtf16(file);
		}
		else
		{
			dst.DiffuseMap.clear();
		}
	}
	void FBXLoader::CreateVertexBuffer()
	{
		int count = 0;
		vertexBuffers_.resize(meshes.size());
		vertexBufferViews_.resize(meshes.size());
		//頂点サイズ
		auto vertexSize = sizeof(Vertex3D);
		for (auto mesh : meshes)
		{
			auto vertex_ = mesh.Vertices;
			vertexBuffers_[count] = std::make_shared<Buffer>();
			vertexBuffers_[count]->Init(pDevice_.get(),vertex_.size()* vertexSize, vertexSize, BufferUsage::VertexBuffer, false, false);
			vertexBufferViews_[count] = std::make_shared<VertexBufferView>();
			vertexBufferViews_[count]->Init(pDevice_.get(), vertexBuffers_[count].get());
			vertexBuffers_[count]->UpdateBuffer(pDevice_.get(), pCmdList_.get(),vertex_.data(), vertex_.size() * vertexSize);
			count++;
		}

	}
	void FBXLoader::CreateIndexBuffer()
	{
		int count = 0;
		indexBuffers_.resize(meshes.size());
		indexBufferViews_.resize(meshes.size());
		auto rST = ResourceStateTracker::Instance();
		//頂点サイズ
		auto indexSize = sizeof(u32);
		for (auto mesh : meshes)
		{
			auto index_ = mesh.Indices;
			indexBuffers_[count] = std::make_shared<Buffer>();
			indexBuffers_[count]->Init(pDevice_.get(), index_.size() * indexSize, indexSize, BufferUsage::IndexBuffer, false, false);
			indexBufferViews_[count] = std::make_shared<IndexBufferView>();
			indexBufferViews_[count]->Init(pDevice_.get(), indexBuffers_[count].get());
			indexBuffers_[count]->UpdateBuffer(pDevice_.get(), pCmdList_.get(), index_.data(), index_.size() * indexSize);

			

			count++;
		}
	}
	void FBXLoader::CreateConstantBuffer()
	{
		constantBuffer_ = std::make_shared<Buffer>();
		constantBuffer_.get()->Init(pDevice_.get(), sizeof(*pMatrix_), sizeof(XMMATRIX), BufferUsage::ConstantBuffer, true, false);
		constBufferView_ = std::make_shared<ConstantBufferView>();
		constBufferView_.get()->Init(pDevice_.get(), constantBuffer_.get());

		pMatrix_ = (XMMATRIX *)constantBuffer_.get()->Map();
		if (!pMatrix_)
		{
			assert(0 && "定数バッファのマップに失敗");
		}
		*pMatrix_ = XMMatrixIdentity();		
		UpdateMatrix();
	}
	void FBXLoader::UpdateMatrix()
	{	
		(*pMatrix_) = object->globalTransform.GetMatrix();
			//XMMatrixTranspose(mat);
	}
	void FBXLoader::CreatePipelineState()
	{
		
		vsShader_ = std::make_shared<Shader>();
		vsShader_->Init(pDevice_.get(), ShaderType::Vertex, "simpleFBX");
		psShader_ = std::make_shared<Shader>();
		psShader_->Init(pDevice_.get(), ShaderType::Pixel, "simpleFBX");
		rootSignature_ = std::make_shared<RootSignature>();
		rootSignature_->Init(pDevice_.get(), vsShader_.get(), psShader_.get(), nullptr, nullptr, nullptr);

		auto bbidx = pDevice_->GetSwapChain().GetFrameIndex();
		auto &swapChain = pDevice_->GetSwapChain();

		D3D12_INPUT_ELEMENT_DESC elementDescs[] = { 
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // float3のPOSITION
	{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // float3のNORMAL
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // float2のTEXCOORD
	{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // float3のTANGENT
	{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // float4のCOLOR
		};

		ym::GraphicsPipelineStateDesc desc;
		desc.rasterizer.fillMode = D3D12_FILL_MODE_SOLID;
		desc.rasterizer.cullMode = D3D12_CULL_MODE_NONE;
		desc.rasterizer.isFrontCCW = false;
		desc.rasterizer.isDepthClipEnable = false;
		desc.multisampleCount = 1;

		desc.blend.sampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		desc.blend.rtDesc[0].isBlendEnable = false;
		desc.blend.rtDesc[0].srcBlendColor = D3D12_BLEND_ZERO;
		desc.blend.rtDesc[0].dstBlendColor = D3D12_BLEND_ZERO;
		desc.blend.rtDesc[0].blendOpColor = D3D12_BLEND_OP_ADD;
		desc.blend.rtDesc[0].srcBlendAlpha = D3D12_BLEND_ZERO;
		desc.blend.rtDesc[0].dstBlendAlpha = D3D12_BLEND_ZERO;
		desc.blend.rtDesc[0].blendOpAlpha = D3D12_BLEND_OP_ADD;
		desc.blend.rtDesc[0].writeMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		desc.depthStencil.depthFunc = D3D12_COMPARISON_FUNC_LESS;
		desc.depthStencil.isDepthEnable = true;
		desc.depthStencil.isDepthWriteEnable = true;

		desc.pRootSignature = rootSignature_.get();
		desc.pVS = vsShader_.get();
		desc.pPS = psShader_.get();
		desc.primTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		
		//auto inputLayout = Vertex::InputLayout;
		desc.inputLayout.numElements = _countof(elementDescs);
		desc.inputLayout.pElements = elementDescs; 
		desc.numRTVs = 0;
		desc.rtvFormats[desc.numRTVs++] = pDevice_->GetSwapChain().GetRenderTargetView(bbidx)->GetFormat();
		desc.dsvFormat = pDevice_->GetSwapChain().GetDepthStencilTexture()->GetTextureDesc().format;
		pipelineState_ = std::make_shared<GraphicsPipelineState>();
		pipelineState_->Init(pDevice_.get(), desc);
	}
	void FBXLoader::CreateSampler()
	{
		D3D12_SAMPLER_DESC desc{};
		desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler_ = std::make_shared<Sampler>();
		sampler_->Initialize(pDevice_.get(), desc);
	}
}
