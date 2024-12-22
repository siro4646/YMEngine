#include "textureView.h"

#include "device/device.h"
#include "texture/texture.h"
#include "descriptor/descriptor.h"


namespace ym
{
	bool RenderTargetView::Init(Device *pDev, Texture *pTex, u32 mipSlice, u32 firstArray, u32 arraySize)
	{
		const D3D12_RESOURCE_DESC &resDesc = pTex->GetResourceDesc();
		D3D12_RENDER_TARGET_VIEW_DESC viewDesc{};
		viewDesc.Format = resDesc.Format;
		if (resDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE1D)
		{
			if (resDesc.DepthOrArraySize == 1)
			{
				viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
				viewDesc.Texture1D.MipSlice = mipSlice;
			}
			else
			{
				viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
				viewDesc.Texture1DArray.MipSlice = mipSlice;
				viewDesc.Texture1DArray.FirstArraySlice = firstArray;
				viewDesc.Texture1DArray.ArraySize = arraySize;
			}
		}
		else if (resDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)
		{
			if (resDesc.SampleDesc.Count == 1)
			{
				if (resDesc.DepthOrArraySize == 1)
				{
					viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
					viewDesc.Texture2D.MipSlice = mipSlice;
					viewDesc.Texture2D.PlaneSlice = 0;
				}
				else
				{
					viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
					viewDesc.Texture2DArray.MipSlice = mipSlice;
					viewDesc.Texture2DArray.PlaneSlice = 0;
					viewDesc.Texture2DArray.FirstArraySlice = firstArray;
					viewDesc.Texture2DArray.ArraySize = arraySize;
				}
			}
			else
			{
				if (resDesc.DepthOrArraySize == 1)
				{
					viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
				}
				else
				{
					viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
					viewDesc.Texture2DMSArray.FirstArraySlice = firstArray;
					viewDesc.Texture2DMSArray.ArraySize = arraySize;
				}
			}
		}
		else if (resDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D)
		{
			viewDesc.Texture3D.MipSlice = mipSlice;
			viewDesc.Texture3D.FirstWSlice = firstArray;
			viewDesc.Texture3D.WSize = arraySize;
		}
		else
		{
			return false;
		}

		descInfo_ = pDev->GetRtvDescriptorHeap().Allocate();
		if (!descInfo_.IsValid())
		{
			return false;
		}

		pDev->GetDeviceDep()->CreateRenderTargetView(pTex->GetResourceDep(), &viewDesc, descInfo_.cpuHandle);
		format_ = viewDesc.Format;

		return true;
	}
	void RenderTargetView::Uninit()
	{
		if (descInfo_.IsValid())
		{
			descInfo_.Free();
		}
	}
}