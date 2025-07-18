#pragma once


#include "descriptorHeap/descriptorHeap.h"


namespace ym
{
	class Device;
	class Buffer;
	class Descriptor;

	//-------------------------------------------------------------------------
	class ConstantBufferView
	{
	public:
		ConstantBufferView()
		{}
		~ConstantBufferView()
		{
			Destroy();
		}

		bool Init(Device *pDev, Buffer *pBuffer, size_t offset = 0, size_t size = 0);
		void Destroy();

		// getter
		DescriptorInfo &GetDescInfo() { return descInfo_; }
		const DescriptorInfo &GetDescInfo() const { return descInfo_; }

	private:
		DescriptorInfo	descInfo_;
	};	// class ConstantBufferView


	//-------------------------------------------------------------------------
	class VertexBufferView
	{
	public:
		VertexBufferView()
		{}
		~VertexBufferView()
		{
			Destroy();
		}

		bool Init(Device *pDev, Buffer *pBuffer, size_t offset = 0, size_t size = 0);
		bool Init(Device *pDev, Buffer *pBuffer, size_t offset, size_t size, size_t stride);
		void Destroy();

		// getter
		const D3D12_VERTEX_BUFFER_VIEW &GetView() const { return view_; }
		size_t GetBufferOffset() const { return bufferOffset_; }

	private:
		D3D12_VERTEX_BUFFER_VIEW	view_{};
		size_t						bufferOffset_ = 0;
	};	// class VertexBufferView


	//-------------------------------------------------------------------------
	class IndexBufferView
	{
	public:
		IndexBufferView()
		{}
		~IndexBufferView()
		{
			Destroy();
		}

		bool Init(Device *pDev, Buffer *pBuffer, size_t offset = 0, size_t size = 0);
		bool Init(Device *pDev, Buffer *pBuffer, size_t offset, size_t size, size_t stride);
		void Destroy();

		// getter
		const D3D12_INDEX_BUFFER_VIEW &GetView() const { return view_; }
		size_t GetBufferOffset() const { return bufferOffset_; }

	private:
		D3D12_INDEX_BUFFER_VIEW		view_{};
		size_t						bufferOffset_ = 0;
	};	// class IndexBufferView


	//----------------------------------------------------------------------------
	class BufferView
	{
	public:
		BufferView()
		{}
		~BufferView()
		{
			Destroy();
		}

		bool Init(Device *pDev, Buffer *pBuffer, u32 firstElement, u32 numElement, u32 stride);
		void Destroy();

		// getter
		const D3D12_SHADER_RESOURCE_VIEW_DESC &GetViewDesc() const { return viewDesc_; }
		DescriptorInfo &GetDescInfo() { return descInfo_; }
		const DescriptorInfo &GetDescInfo() const { return descInfo_; }

	private:
		D3D12_SHADER_RESOURCE_VIEW_DESC	viewDesc_;
		DescriptorInfo					descInfo_;
	};	// class BufferView

}	// namespace ym

//	EOF
