#pragma once

#include "descriptorHeap/descriptorHeap.h"

namespace ym
{
	class Device;
	class Texture;

	class RenderTargetView
	{
	public:
		RenderTargetView()
		{}
		~RenderTargetView()
		{
			Uninit();
		}
		bool Init(Device *pDev, Texture *pTex, u32 mipSlice = 0, u32 firstArray = 0, u32 arraySize = 1);
		void Uninit();

		//Getter
		const DescriptorInfo &GetDescInfo() const { return descInfo_; }
		DXGI_FORMAT	GetFormat() { return format_; }
	private:
		DXGI_FORMAT		format_{ DXGI_FORMAT_UNKNOWN };
		DescriptorInfo	descInfo_;

	};

}