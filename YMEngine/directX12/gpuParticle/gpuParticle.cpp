#include "gpuParticle.h"

#include "Renderer/renderer.h"
#include "device/device.h"
#include "commandList/commandList.h"
#include "pipelineState/pipelineState.h"
#include "descriptorSet/DescriptorSet.h"
#include "shader/shader.h"
#include "rootSignature/rootSignature.h"

#include "buffer/buffer.h"
#include "bufferView/bufferView.h"
#include "texture/texture.h"
#include "textureView/textureView.h"


void ym::ParticleSystem::Init()
{
	renderer_ = Renderer::Instance();
	device_ = renderer_->GetDevice();
	computeCommandList_ = renderer_->GetComputeCommandList();

	descriptorSet_ = std::make_shared<DescriptorSet>();
	CreateShader();
	CreateRootSignature();
	CreatePipelineState();

	CreateSystemBuffer();
	CreateParticleHeaderBuffer();

	SystemInit();
}

void ym::ParticleSystem::Update()
{
	SystemInit();
}

void ym::ParticleSystem::Uninit()
{
	cs_.reset();
	rootSignature_.reset();
	sytemPipelineState_.reset();

	systemBuffer_.reset();
	systemBufferView_.reset();

	phBuffer_.reset();
	phBufferView_.reset();

}

void ym::ParticleSystem::SystemInit()
{
	auto cmdList = computeCommandList_->GetCommandList();
	auto ds = descriptorSet_.get();

	computeCommandList_->Reset();

	//パーティクルシステムの初期化
	ds->Reset();

	//バリアの設定
	computeCommandList_->TransitionBarrier(systemBuffer_.get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	ds->SetCsUav(0, systemBufferView_->GetDescInfo().cpuHandle);
	cmdList->SetPipelineState(sytemPipelineState_->GetPSO());
	computeCommandList_->SetComputeRootSignatureAndDescriptorSet(rootSignature_.get(), descriptorSet_.get());
	cmdList->Dispatch(1, 1, 1);

	computeCommandList_->UAVBarrier(systemBuffer_.get());

	computeCommandList_->TransitionBarrier(systemBuffer_.get(), D3D12_RESOURCE_STATE_COPY_SOURCE);

	//パーティクルヘッダーの初期化
	ds->Reset();

	//バリアの設定
	computeCommandList_->TransitionBarrier(phBuffer_.get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	ds->SetCsUav(1, phBufferView_->GetDescInfo().cpuHandle);
	cmdList->SetPipelineState(phPipelineState_->GetPSO());
	computeCommandList_->SetComputeRootSignatureAndDescriptorSet(rootSignature_.get(), descriptorSet_.get());

	// パーティクル数に応じたスレッドグループ数を計算
	cmdList->Dispatch(ceil(MAX_PARTICLES/(float)PARTICLE_PER_THREAD), 1, 1);

	computeCommandList_->UAVBarrier(phBuffer_.get());

	computeCommandList_->TransitionBarrier(phBuffer_.get(), D3D12_RESOURCE_STATE_COPY_SOURCE);


	computeCommandList_->Close();
	computeCommandList_->Execute();

	device_->WaitForComputeCommandQueue();

	computeCommandList_->Reset();


}

void ym::ParticleSystem::CreateShader()
{
	cs_ = std::make_shared<Shader>();
	cs_->Init(device_, ShaderType::Compute, "gpuParticle");
}

void ym::ParticleSystem::CreateRootSignature()
{
	rootSignature_ = std::make_shared<RootSignature>();
	rootSignature_->Init(device_, cs_.get());
}

void ym::ParticleSystem::CreatePipelineState()
{
	sytemPipelineState_ = std::make_shared<ComputePipelineState>();
	ComputePipelineStateDesc desc;
	desc.pRootSignature = rootSignature_.get();
	desc.pCS = cs_.get();
	sytemPipelineState_->InitFromEntryPoint(device_, desc, "ClearSystem");

	phPipelineState_ = std::make_shared<ComputePipelineState>();
	phPipelineState_->InitFromEntryPoint(device_, desc, "ClearParticles");
}

void ym::ParticleSystem::CreateSystemBuffer()
{
	systemBuffer_ = std::make_shared<Buffer>();
	systemBuffer_->Init(device_, sizeof(UINT)*2, sizeof(UINT), BufferUsage::ShaderResource, false, true);

	systemBufferView_ = std::make_shared<UnorderedAccessView>();
	systemBufferView_->Initialize(device_, systemBuffer_.get(), 0,2, sizeof(UINT), 0);
}

void ym::ParticleSystem::CreateParticleHeaderBuffer()
{
	phBuffer_ = std::make_shared<Buffer>();
	phBuffer_->Init(device_, sizeof(ParticleHeader) * MAX_PARTICLES, sizeof(ParticleHeader), BufferUsage::ShaderResource, false, true);

	phBufferView_ = std::make_shared<UnorderedAccessView>();
	phBufferView_->Initialize(device_, phBuffer_.get(), 0, MAX_PARTICLES, sizeof(ParticleHeader), 0);
}
