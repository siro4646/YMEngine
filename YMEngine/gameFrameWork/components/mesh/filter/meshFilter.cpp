#include "meshFilter.h"
#include "../instance/meshInstance.h"
#include "../renderer/meshRenderer.h"

#include "Renderer/renderer.h"
#include "device/device.h"
#include "commandList/commandList.h"
#include "descriptorSet/DescriptorSet.h"
#include "buffer/buffer.h"
#include "bufferView/bufferView.h"

#include "camera/camera.h"
#include "camera/cameraManager.h"

#include "../renderer/meshRenderer.h"

namespace ym
{
	namespace mesh
	{
		void MeshFilter::Init()
		{
			// 初期化時の処理（必要に応じて）
			renderer_ = Renderer::Instance();
			if (renderer_)
			{
				device_ = renderer_->GetDevice();
				cmdList_ = renderer_->GetGraphicCommandList();
			}
			CreateConstantBuffer();
			pCamera_ = CameraManager::Instance().GetMainCamera();
		}

		void MeshFilter::Update()
		{
			UpdateMatrix();
		}

		void MeshFilter::Uninit()
		{
			meshInstance_.reset();
			vertexBuffer_.reset();
			vbView_.reset();
			indexBuffer_.reset();
			ibView_.reset();
			renderer_ = nullptr;
			device_ = nullptr;
			cmdList_ = nullptr;

			constantBuffer_.reset();
			constBufferView_.reset();
			pMatrix_ = nullptr;
			pCamera_ = nullptr;
		}

		void MeshFilter::SetMeshInstance(std::shared_ptr<MeshInstance> instance)
		{
			meshInstance_ = instance;
			CreateVertexBuffer();
			CreateIndexBuffer();

			//変更したらMehsRendererに通知
			auto meshRenderer = GetComponent<MeshRenderer>();
			if (meshRenderer)
			{
				//MeshRendererに自分を渡す
				meshRenderer->SetMeshFilter(this);
			}
		}

		std::shared_ptr<MeshInstance> MeshFilter::GetMeshInstance() const
		{
			return meshInstance_;
		}

		void MeshFilter::DrawImguiBody()
		{
			if (meshInstance_)
			{
				ImGui::Text("MeshInstance: %p", meshInstance_.get());

				const auto &mesh = meshInstance_->GetMesh();

				// 頂点数表示
				ImGui::Text("Vertex Count: %zu", mesh.Vertices.size());
				//バッファの内容を表示
				ImGui::Text("Vertex Buffer Size: %zu bytes", vertexBuffer_ ? vertexBuffer_->GetSize() : 0);
				ImGui::Text("Vertex Stride: %zu bytes", vertexBuffer_ ? vertexBuffer_->GetStride() : 0);
				//ビューの情報を表示				
				ImGui::Text("Vertex Buffer View Offset: %zu", vbView_ ? vbView_->GetBufferOffset() : 0);

				if (ImGui::TreeNode("Vertices"))
				{
					for (size_t i = 0; i < mesh.Vertices.size(); ++i)
					{
						const auto &v = mesh.Vertices[i];
						ImGui::Text("[%zu] Pos(%.2f, %.2f, %.2f), Normal(%.2f, %.2f, %.2f), UV(%.2f, %.2f)",
							i,
							v.Position.x, v.Position.y, v.Position.z,
							v.Normal.x, v.Normal.y, v.Normal.z,
							v.UV.x, v.UV.y);

						// 表示数を制限（重いので）
						if (i >= 10)
						{
							ImGui::TextDisabled("...and more");
							break;
						}
					}
					
					ImGui::TreePop();
				}

				// インデックス数表示
				ImGui::Text("Index Count: %zu", mesh.Indices.size());
				//バッファの内容を表示
				ImGui::Text("Index Buffer Size: %zu bytes", indexBuffer_ ? indexBuffer_->GetSize() : 0);
				ImGui::Text("Index Stride: %zu bytes", indexBuffer_ ? indexBuffer_->GetStride() : 0);
				//ビューの情報を表示
				ImGui::Text("Index Buffer View Offset: %zu", ibView_ ? ibView_->GetBufferOffset() : 0);


				if (ImGui::TreeNode("Indices"))
				{
					for (size_t i = 0; i < mesh.Indices.size(); i += 3)
					{
						if (i + 2 < mesh.Indices.size())
						{
							ImGui::Text("[%zu-%zu] %u, %u, %u", i, i + 2,
								mesh.Indices[i], mesh.Indices[i + 1], mesh.Indices[i + 2]);
						}
						else
						{
							ImGui::Text("[%zu] %u", i, mesh.Indices[i]);
						}

						if (i >= 30) // 最大10面だけ表示
						{
							ImGui::TextDisabled("...and more");
							break;
						}
					}
					ImGui::TreePop();
				}
			}
			else
			{
				ImGui::TextDisabled("No MeshInstance assigned");
			}
		}

		void MeshFilter::ApplyBuffers(VertexBufferView *vb, IndexBufferView *iv)
		{
			auto cmdList = cmdList_->GetCommandList();

			cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			if (vb)
			{
				cmdList->IASetVertexBuffers(0, 1, &vb->GetView());
			}
			else if (vertexBuffer_)
			{
				cmdList->IASetVertexBuffers(0, 1, &vbView_->GetView());
			}

			if (iv)
			{
				cmdList->IASetIndexBuffer(&iv->GetView());
			}
			else if (indexBuffer_)
			{
				cmdList->IASetIndexBuffer(&ibView_->GetView());
			}
		}
		

		void MeshFilter::ApplyConstantBuffers(DescriptorSet *ds)
		{
			ds->SetVsCbv(0, constBufferView_->GetDescInfo().cpuHandle);
			ds->SetVsCbv(1, pCamera_->GetDescriptorHandle());
		}

		void MeshFilter::CreateVertexBuffer()
		{
			auto vertexSize = sizeof(Vertex3D);
			auto bufferSize = meshInstance_->GetMesh().Vertices.size() * vertexSize;
			vertexBuffer_ = std::make_shared<Buffer>();
			vertexBuffer_->Init(device_, bufferSize, vertexSize, BufferUsage::VertexBuffer, false, false);
			vbView_ = std::make_shared<VertexBufferView>();
			vbView_->Init(device_, vertexBuffer_.get());
			vertexBuffer_->UpdateBuffer(device_, cmdList_, meshInstance_->GetMesh().Vertices.data(), bufferSize);
		}

		void MeshFilter::CreateIndexBuffer()
		{
			auto indexSize = sizeof(u32);
			auto bufferSize = meshInstance_->GetMesh().Indices.size() * indexSize;
			indexBuffer_ = std::make_shared<Buffer>();
			indexBuffer_->Init(device_, bufferSize, indexSize, BufferUsage::IndexBuffer, false, false);
			ibView_ = std::make_shared<IndexBufferView>();
			ibView_->Init(device_, indexBuffer_.get());
			indexBuffer_->UpdateBuffer(device_, cmdList_, meshInstance_->GetMesh().Indices.data(), bufferSize);
		}

		void MeshFilter::CreateConstantBuffer()
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

		void MeshFilter::UpdateMatrix()
		{
			(*pMatrix_) = object->worldTransform.GetMatrix();
		}

	}
}