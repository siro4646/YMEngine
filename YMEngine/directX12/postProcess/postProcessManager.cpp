#include "postProcessManager.h"

#include "postProcess/postProcessMaterial.h"

#include "device/device.h"
#include "commandList/commandList.h"
#include "Renderer/renderer.h"

#include "buffer/buffer.h"
#include "bufferView/bufferView.h"

#include "texture/texture.h"
#include "textureView/textureView.h"

namespace ym
{
	void PostProcessManager::Init()
	{
		auto renderer = Renderer::Instance();
		device_ = renderer->GetDevice();
		graphicsCmdList_ = renderer->GetGraphicCommandList();

		resultRTVs_.resize(SwapChain::kFrameCount);
		resultTextures_.resize(SwapChain::kFrameCount);
		resultTextureViews_.resize(SwapChain::kFrameCount);

		vertexBuffer_ = std::make_shared<Buffer>();
		vertexBufferView_ = std::make_shared<VertexBufferView>();
		CreateVertexBuffer();

		indexBuffer_ = std::make_shared<Buffer>();
		indexBufferView_ = std::make_shared<IndexBufferView>();
		CreateIndexBuffer();

		////�����_���[�̃V�[��RTV,�e�N�X�`��,�e�N�X�`���r���[�����U���g�ɃR�s�[
		//for (int i = 0; i < SwapChain::kFrameCount; ++i)
		//{
		//	auto sceneRTV = renderer->GetSceneRenderTargetView(i);
		//	resultRTVs_[i] = sceneRTV;	
		//	auto sceneTexture = renderer->GetSceneRenderTexture(i);
		//	resultTextures_[i] = sceneTexture;
		//	auto sceneTextureView = renderer->GetSceneRenderTargetTexView(i);
		//	resultTextureViews_[i] = sceneTextureView;			
		//}

		//auto sceneRTV = renderer->GetSceneRenderTargetView();


	}
	void PostProcessManager::Uninit()
	{
		vertexBuffer_.reset();
		vertexBufferView_.reset();
		indexBuffer_.reset();
		indexBufferView_.reset();
		for (auto material : materials_)
		{
			material->Uninit();
		}
		materials_.clear();
		resultRTVs_.clear();
		resultTextures_.clear();
		resultTextureViews_.clear();



	}
	void PostProcessManager::Update()
	{
	}
	void PostProcessManager::Draw()
	{
		//�����_�[�^�[�Q�b�g�����U���g�ɃR�s�[
		auto bbidx = device_->GetSwapChain().GetFrameIndex();
		auto cmdList = graphicsCmdList_;


		resultRTVs_[bbidx] = Renderer::Instance()->GetSceneRenderTargetView(bbidx);
		resultTextures_[bbidx] = Renderer::Instance()->GetSceneRenderTexture(bbidx);
		resultTextureViews_[bbidx] = Renderer::Instance()->GetSceneRenderTargetTexView(bbidx);
		for (auto material : materials_)
		{
			//���[�g�V�O�l�`���ȂǃV�F�[�_�[�ɓn�������Z�b�g
			material->SetMaterial();
			//�`��
			cmdList->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			cmdList->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_->GetView());
			cmdList->GetCommandList()->IASetIndexBuffer(&indexBufferView_->GetView());
			cmdList->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

			//�`�悵���}�e���A���̃o���A��J��
			cmdList->TransitionBarrier(material->GetTexture(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

			//�`�悵�����̂����U���g�ɃR�s�[
			resultRTVs_[bbidx] = material->GetRTV();
			resultTextures_[bbidx] = material->GetTexture();
			resultTextureViews_[bbidx] = material->GetTextureView();
		}
	}
	void PostProcessManager::AddPostProcessMaterial(PostProcessMaterial *material)
	{
		//���łɓo�^����Ă��邩�`�F�b�N
		for (auto mat : materials_)
		{
			if (mat == material)
			{
				return;
			}
		}

		materials_.push_back(material);
	}
	void PostProcessManager::RemovePostProcessMaterial(PostProcessMaterial *material)
	{
		//�Ȃ��Ȃ牽�����Ȃ�
		if (std::find(materials_.begin(), materials_.end(), material) == materials_.end())
		{
			return;
		}

		materials_.erase(std::remove(materials_.begin(), materials_.end(), material), materials_.end());
	}
	void PostProcessManager::CreateVertexBuffer()
	{
		vertexBuffer_->Init(device_, sizeof(vertices_), sizeof(Vertex2D), BufferUsage::VertexBuffer, false, false);
		vertexBufferView_->Init(device_, vertexBuffer_.get());
		vertexBuffer_->UpdateBuffer(device_, graphicsCmdList_, &vertices_, sizeof(vertices_));
	}
	void PostProcessManager::CreateIndexBuffer()
	{
		indexBuffer_->Init(device_, sizeof(indices_), sizeof(u32), BufferUsage::IndexBuffer, false, false);
		indexBufferView_->Init(device_, indexBuffer_.get());
		indexBuffer_->UpdateBuffer(device_, graphicsCmdList_, &indices_, sizeof(indices_));
	}
}
