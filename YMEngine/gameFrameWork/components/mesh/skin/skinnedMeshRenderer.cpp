#include "skinnedMeshRenderer.h"
#include "skeleton.h"


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
		void SkinnedMeshRenderer::Init()
		{
			// 初期化時の処理（必要に応じて）
			renderer_ = Renderer::Instance();
			if (renderer_)
			{
				device_ = renderer_->GetDevice();
				cmdList_ = renderer_->GetGraphicCommandList();
			}
			// MeshFilterの取得
			meshFilter_ = GetComponent<MeshFilter>();

			descriptorSet_ = std::make_shared<DescriptorSet>();

			CreateBuffer();

			//auto mat = std::make_shared<ym::material::Material>();
			//mat->Init("pbrVS", "pbrPS");
			//mat->SetTexture("g_texture", "aaa");
			//mat->SetTexture("g_maskTexture", "white");
			////material_->Init(); // シェーダーの初期化
			//mat->SetTexture("g_texture", "aaa"); // テクスチャの設定
			//mat->SetTexture("g_maskTexture", "white"); // マスクテクスチャの設定
			//mat->SetMatrix("mat", object->worldTransform.GetMatrix());
			//mat->SetMatrix("view", CameraManager::Instance().GetMainCamera()->GetViewMatrix());
			//mat->SetMatrix("proj", CameraManager::Instance().GetMainCamera()->GetProjectionMatrix());
			//mat->SetMatrix("invView", CameraManager::Instance().GetMainCamera()->GetInvViewMatrix()); // 逆ビュー行列
			//mat->SetMatrix("invProj", CameraManager::Instance().GetMainCamera()->GetInvProjectionMatrix()); // 逆プロジェクション行列
			//mat->SetMatrix("invViewProj", CameraManager::Instance().GetMainCamera()->GetInvViewProjectionMatrix());
			//materials_.push_back(mat);
		}

		void SkinnedMeshRenderer::FixedUpdate()
		{
			//
			//UpdateSkinning(); // スキニングの更新
		}

		void SkinnedMeshRenderer::Uninit()
		{
			meshFilter_.reset();
			skeleton_.reset();
			for (auto &mat : materials_)
			{
				if (mat) {
					//mat->Uninit(); // マテリアルの解放
					mat.reset(); // スマートポインタのリセット
				}
			}
			materials_.clear();
			descriptorSet_.reset();
			cmdList_ = nullptr;
			renderer_ = nullptr;
			device_ = nullptr;
			skinnedVertexBuffer_.reset();
			currentBoneMatrices_.clear();
		}

		void SkinnedMeshRenderer::DrawImguiBody()
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
		}

		void SkinnedMeshRenderer::SetSkeleton(std::shared_ptr<Skeleton> skel)
		{
			skeleton_ = skel;
			if (skeleton_)
			{
				currentBoneMatrices_.resize(skeleton_->joints.size());
			}
			else
			{
				currentBoneMatrices_.clear();
			}
		}

		void SkinnedMeshRenderer::SetMeshFilter(std::shared_ptr<MeshFilter> filter)
		{
			meshFilter_ = filter;
			if (meshFilter_)
			{
				skinnedVertexBuffer_.reset(); // 既存のバッファをクリア
				skinnedVertexBufferView_.reset(); // 既存のビューをクリア
				//currentBoneMatrices_.clear(); // ボーンマトリクスもクリア
				// MeshFilterが設定されたら、スキン済み頂点バッファを初期化
				//skinnedVertexBuffer_ = std::make_shared<Buffer>();
				//currentBoneMatrices_.resize(meshFilter_->GetMeshInstance()->GetMesh().Bones.size());
			}
		}

		// -------------------------------------------------------------
// SkinnedMeshRenderer::UpdateSkinning
// -------------------------------------------------------------
		void SkinnedMeshRenderer::UpdateSkinning()
		{
			if (!meshFilter_ || !skeleton_) return;

			const auto &mesh = meshFilter_->GetMeshInstance()->GetMesh();
			const auto &vertices = mesh.Vertices;
			const auto &weights = mesh.BoneWeights;
			const auto &invBinds = mesh.BindPoseInverse;   // ← こっちを使う

			// ── ① グローバル bindPose を構築 ──
			size_t J = skeleton_->joints.size();
			if (currentBoneMatrices_.size() != J)
				currentBoneMatrices_.resize(J);

			std::vector<XMMATRIX> globalBind(J);
			for (size_t i = 0; i < J; ++i)
			{
				XMMATRIX local = XMLoadFloat4x4(&skeleton_->joints[i].localBindPose);
				int parent = skeleton_->joints[i].parentIndex;
				globalBind[i] = (parent >= 0)
					? local * globalBind[parent]
					: local;
			}

			// ── ② スキニング行列を生成 ──
			for (size_t i = 0; i < J; ++i)
			{
				// globalBindPose[i] は既に計算済み
				XMMATRIX global = globalBind[i];

				// ← mesh 側ではなく SkeletonJoint が持つ元祖逆バインドを使う
				XMMATRIX invBind = XMLoadFloat4x4(&skeleton_->joints[i].inverseBindPose);

				XMMATRIX skinMat = global * invBind;
				XMStoreFloat4x4(&currentBoneMatrices_[i], skinMat);
			}

			// ── ③ 頂点を CPU スキニング ──
			std::vector<Vertex3D> skinned(vertices.size());
			for (size_t v = 0; v < vertices.size(); ++v)
			{
				XMVECTOR pos = XMLoadFloat3(&vertices[v].Position);
				XMVECTOR norm = XMLoadFloat3(&vertices[v].Normal);

				XMVECTOR p_acc = XMVectorZero();
				XMVECTOR n_acc = XMVectorZero();

				for (int j = 0; j < 4; ++j)
				{
					float w = weights[v].weights[j];
					if (w <= 0) continue;

					int bi = weights[v].indices[j];
					XMMATRIX skin = XMLoadFloat4x4(&currentBoneMatrices_[bi]);

					// 位置は全変換行列で
					p_acc += XMVector3Transform(pos, skin) * w;

					// 法線は回転部分のみを取り出して正規化
					n_acc += XMVector3TransformNormal(norm, skin) * w;
				}

				XMStoreFloat3(&skinned[v].Position, p_acc);
				XMStoreFloat3(&skinned[v].Normal, XMVector3Normalize(n_acc));

				// その他はそのままコピー
				skinned[v].UV = vertices[v].UV;
				skinned[v].Tangent = vertices[v].Tangent;
				skinned[v].Color = vertices[v].Color;
			}

			// ── ④ GPU バッファへアップロード ──
			size_t byteSize = skinned.size() * sizeof(Vertex3D);
			if (!skinnedVertexBuffer_)
			{
				skinnedVertexBuffer_ = std::make_shared<Buffer>();
				skinnedVertexBuffer_->Init(device_, byteSize, sizeof(Vertex3D),
					BufferUsage::VertexBuffer, false, false);

				skinnedVertexBufferView_ = std::make_shared<VertexBufferView>();
				skinnedVertexBufferView_->Init(device_, skinnedVertexBuffer_.get());
			}
			skinnedVertexBuffer_->UpdateBuffer(device_, cmdList_,
				skinned.data(), byteSize);
		}


		void SkinnedMeshRenderer::CreateBuffer()
		{
			auto vertexSize = sizeof(Vertex3D);
			auto bufferSize = meshFilter_->GetMeshInstance()->GetMesh().Vertices.size() * vertexSize;
			skinnedVertexBuffer_ = std::make_shared<Buffer>();
			skinnedVertexBuffer_->Init(device_, bufferSize, vertexSize, BufferUsage::VertexBuffer, false, false);
			skinnedVertexBufferView_ = std::make_shared<VertexBufferView>();
			skinnedVertexBufferView_->Init(device_, skinnedVertexBuffer_.get());
		}


		void SkinnedMeshRenderer::Update()
		{
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
			/*for (int i = 0; i < skeleton_->joints.size(); ++i)
			{
				const auto &m = skeleton_->joints[i].localBindPose;
				if (m._11 == 1.0f && m._22 == 1.0f && m._33 == 1.0f && m._14 == 0.0f && m._24 == 0.0f && m._34 == 0.0f)
					DebugLog("!! Identity matrix detected at joint %s\n", skeleton_->joints[i].name.c_str());
			}*/

			// スキニングの更新
			//UpdateSkinning();
		}

		void SkinnedMeshRenderer::Draw()
		{
			if (!meshFilter_) return;

			const auto &mesh = meshFilter_->GetMeshInstance()->GetMesh();
			const auto &subs = mesh.SubMeshes;

			auto cmd = cmdList_->GetCommandList();
			meshFilter_->ApplyBuffers(skinnedVertexBufferView_.get());                // ★ VBO / IBO を 1回だけセット

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

	}
}