#include "objLoader.h"
#include "gameFrameWork/components/fbxLoader/fbxLoader.h"

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

#include "material/material.h"
#include "material/objMaterial.h"

#include "resource/resourceManager.h"


namespace ym
{
	void OBJLoader::Init()
	{	
		//既にFBXLoaderまたはObjLoaderがアタッチされている場合は自分を削除する
		

		CreateConstantBuffer();
		//descriptorSet_ = std::make_shared<DescriptorSet>();

		pCamera_ = CameraManager::Instance().GetMainCamera();
		if (!pCamera_)
		{
			ym::ConsoleLog("カメラが設定されていません\n");
		}
	}
	void OBJLoader::FixedUpdate()
	{
	}
	void OBJLoader::Update()
	{
		if (!isInit_) return;

		for (auto material : material_)
		{
			material->Update();
		}
		UpdateMatrix();
	}
	void OBJLoader::Draw()
	{
		if (!isInit_) return;
		auto cmdList = pCmdList_->GetCommandList();
		pCamera_ = CameraManager::Instance().GetMainCamera();

		for (int i = 0; i < meshes.size(); i++)
		{
			material_[i]->SetMaterial();
			material_[i]->SetVsCbv(0, constBufferView_->GetDescInfo().cpuHandle);
			material_[i]->SetVsCbv(1, pCamera_->GetDescriptorHandle());
			material_[i]->Draw();	
		}
	}

	void OBJLoader::DrawImguiBody()
	{
		ImGui::Text("OBJ Loader Info");

		if (ImGui::TreeNode("Meshes & Materials"))
		{
			for (size_t i = 0; i < meshes.size(); ++i)
			{
				std::string label = "Mesh[" + std::to_string(i) + "]";
				if (ImGui::TreeNode(label.c_str()))
				{
					ImGui::Text("Vertices: %zu", meshes[i].Vertices.size());
					ImGui::Text("Indices: %zu", meshes[i].Indices.size());

					if (i < material_.size() && material_[i])
					{
						string className = ym::GetShortClassName(*material_[i]);
						ImGui::Text("Material: %s", className.c_str());
					}
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
	}


	void OBJLoader::Uninit()
	{
	
		constantBuffer_.reset();
		constBufferView_.reset();
		isInit_ = false;
		meshes.clear();
		material_.clear();
	}
	void OBJLoader::SetMaterial(std::shared_ptr<Material>material, u32 index)
	{
		material_[index] = material;
		material_[index]->CreateMaterial(meshes[index]);
	}
	std::vector<Mesh> OBJLoader::Load(ImportSettings settings)
	{
		//
		if (settings.filename == nullptr)
		{
			return std::vector<Mesh>();
		}
		auto resourceManager = ResourceManager::Instance();
		string sFileName = ym::Utf16ToUtf8(settings.filename);
		if (resourceManager->FindMesh(sFileName))
		{
			auto buffers = resourceManager->GetMesh(sFileName);
			meshes.resize(buffers.size());
			meshes = resourceManager->GetMesh(sFileName);
			buffers.clear();
			//return meshes;
		}
		else
		{

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
				return std::vector<Mesh>();
			}

			// 読み込んだデータを自分で定義したMesh構造体に変換する
			meshes.clear();
			meshes.resize(scene->mNumMeshes);
			/*texturePaths_.resize(scene->mNumMeshes);
			textures_.resize(scene->mNumMeshes);
			textureViews_.resize(scene->mNumMeshes);
			specTexturePaths_.resize(scene->mNumMeshes);
			specTextures_.resize(scene->mNumMeshes);
			specTextureViews_.resize(scene->mNumMeshes);
			maskexturePaths_.resize(scene->mNumMeshes);
			maskTextures_.resize(scene->mNumMeshes);
			maskTextureViews_.resize(scene->mNumMeshes);*/
			//======================================
			//======================================
			for (size_t i = 0; i < meshes.size(); ++i)
			{
				const auto pMesh = scene->mMeshes[i];
				LoadMesh(meshes[i], pMesh, inverseU, inverseV);
				const auto pMaterial = scene->mMaterials[i];
				LoadTexture(settings.filename, meshes[i], pMaterial);
			}
			scene = nullptr;
		}
		material_.resize(meshes.size());
		NormalizeScale();

		for (size_t i = 0; i < meshes.size(); ++i)
		{
			//======================================
			material_[i] = std::make_shared<OBJMaterial>();
			material_[i]->CreateMaterial(meshes[i]);
			//======================================
		}


		
		isInit_ = true;


		pCmdList_->Close();
		pCmdList_->Execute();
		pDevice_->WaitForGraphicsCommandQueue();
		
		pCmdList_->Reset();	

		resourceManager->SetMesh(sFileName, meshes);
		return meshes;

	}
	void OBJLoader::LoadMesh(Mesh &dst, const aiMesh *src, bool inverseU, bool inverseV)
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
	void OBJLoader::NormalizeScale()
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
	void OBJLoader::LoadTexture(const wchar_t *filename, Mesh &dst, const aiMaterial *src)
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
		if (src->Get(AI_MATKEY_TEXTURE_SPECULAR(0), path) == AI_SUCCESS)
		{
			auto dir = GetDirectoryPath(filename);
			auto file = std::string(path.C_Str());
			dst.SpecularMap = dir + ym::Utf8ToUtf16(file);
		}
		else
		{
			dst.SpecularMap.clear();
			dst.SpecularMap = L"black";
		}
		//マスクテクスチャがある場合
		if (src->Get(AI_MATKEY_TEXTURE_OPACITY(0), path) == AI_SUCCESS)
		{
			auto dir = GetDirectoryPath(filename);
			auto file = std::string(path.C_Str());
			dst.MaskMap = dir + ym::Utf8ToUtf16(file);
		}
		else
		{
			dst.MaskMap.clear();
			dst.MaskMap = L"white";
		}

	}

	void OBJLoader::CreateConstantBuffer()
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
	void OBJLoader::UpdateMatrix()
	{
		if (transform_ )
		{
			(*pMatrix_) = transform_->GetMatrix();
		}
		else {
			(*pMatrix_) = object->worldTransform.GetMatrix();
		}
	}
	
}
