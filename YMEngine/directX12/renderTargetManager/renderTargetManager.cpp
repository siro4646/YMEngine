#include "renderTargetManager.h"

#include "Renderer/renderer.h"
#include "device/device.h"
#include "commandList/commandList.h"

void ym::RenderTargetManager::Init()
{
	cmdList_ = Renderer::Instance()->GetGraphicCommandList();
}

void ym::RenderTargetManager::Uninit()
{
	cmdList_ = nullptr;/*
	for (auto &rtv : prvRtvs_)
	{
		rtv.Uninit();
	}*/
	prvRtvs_.clear();
	prvDsv_ = nullptr;
	prvDst_ = nullptr;
}

void ym::RenderTargetManager::BeginRenderTarget(std::vector<RenderTargetView>rtvs, std::vector<Texture>rts, DepthStencilView *dsv, Texture *dst, bool clearFlag)
{
	prvRtvs_.resize(rtvs.size());
	for (u32 i = 0; i < rtvs.size(); ++i)
	{
		prvRtvs_[i] = rtvs[i];
	}
	prvDsv_ = dsv;
	prvDst_ = dst;
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>rtvsHandles;
	for (auto &rtv : rtvs)
	{
		rtvsHandles.push_back(rtv.GetDescInfo().cpuHandle);
	}
	//•¡”ŒÂ‚ÌRTV‚ª“n‚³‚ê‚½‚çflag‚ðtrue‚É‚·‚é
	bool rtvFlag = rtvs.size() > 1 ? true : false;
	if(!dsv)
		cmdList_->GetCommandList()->OMSetRenderTargets(rtvs.size(), rtvsHandles.data(), rtvFlag, nullptr);
	else
	{
		cmdList_->GetCommandList()->OMSetRenderTargets(rtvs.size(), rtvsHandles.data(), rtvFlag, &dsv->GetDescInfo().cpuHandle);
	}
	if (clearFlag)
	{
		int i = 0;
		for (auto &rtv : rtvs)
		{
			cmdList_->GetCommandList()->ClearRenderTargetView(rtv.GetDescInfo().cpuHandle,rts[i].GetTextureDesc().clearColor, 0, nullptr);
			++i;
		}
		if (dst && dsv)
		{
			cmdList_->GetCommandList()->ClearDepthStencilView(dsv->GetDescInfo().cpuHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, dst->GetTextureDesc().clearDepth, dst->GetTextureDesc().clearStencil, 0, nullptr);
		}
	}

}

void ym::RenderTargetManager::PushRenderTarget(std::vector<RenderTargetView> rtvs, std::vector<Texture> rts, DepthStencilView *dsv, Texture *dst, bool clearFlag)
{
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>rtvsHandles;
	for (auto &rtv : rtvs)
	{
		rtvsHandles.push_back(rtv.GetDescInfo().cpuHandle);
	}
	//•¡”ŒÂ‚ÌRTV‚ª“n‚³‚ê‚½‚çflag‚ðtrue‚É‚·‚é
	bool rtvFlag = rtvs.size() > 1 ? true : false;

	if (!dsv)
	{
		cmdList_->GetCommandList()->OMSetRenderTargets(rtvs.size(), rtvsHandles.data(), rtvFlag, nullptr);
	}
	else
	{
		cmdList_->GetCommandList()->OMSetRenderTargets(rtvs.size(), rtvsHandles.data(), rtvFlag, &dsv->GetDescInfo().cpuHandle);
	}
	if (clearFlag)
	{
		int i = 0;
		for (auto &rtv : rtvs)
		{
			cmdList_->GetCommandList()->ClearRenderTargetView(rtv.GetDescInfo().cpuHandle, rts[i].GetTextureDesc().clearColor, 0, nullptr);
			++i;
		}
		if (dst && dsv)
		cmdList_->GetCommandList()->ClearDepthStencilView(dsv->GetDescInfo().cpuHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,dst->GetTextureDesc().clearDepth,dst->GetTextureDesc().clearStencil, 0, nullptr);
	}
}

void ym::RenderTargetManager::PopRenderTarget()
{
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>rtvsHandles;
	for (auto &rtv : prvRtvs_)
	{
		rtvsHandles.push_back(rtv.GetDescInfo().cpuHandle);
	}
	if(prvDsv_)
		cmdList_->GetCommandList()->OMSetRenderTargets(prvRtvs_.size(), rtvsHandles.data(), true, &prvDsv_->GetDescInfo().cpuHandle);
	else
		cmdList_->GetCommandList()->OMSetRenderTargets(prvRtvs_.size(), rtvsHandles.data(), true, nullptr);
}
