
#include "rootSignature.h"
#include "device/device.h"
#include "shader/shader.h"
#include "descriptorHeap/descriptorHeap.h"
#include "descriptorSet/descriptorSet.h"

namespace ym
{
	//----
	bool RootSignature::Init(Device *pDev, const RootSignatureDesc &desc)
	{
		static const u32 kMaxParameters = 64;
		D3D12_DESCRIPTOR_RANGE ranges[kMaxParameters];
		D3D12_ROOT_PARAMETER rootParameters[kMaxParameters];

		auto getRangeFunc = [&](u32 index)
			{
				static const D3D12_DESCRIPTOR_RANGE_TYPE kType[] = {
					D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
					D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
					D3D12_DESCRIPTOR_RANGE_TYPE_UAV,
					D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER,
				};
				return kType[desc.pParameters[index].type];
			};
		auto getShaderVisFunc = [&](u32 index)
			{
				switch (desc.pParameters[index].shaderVisibility)
				{
				case ShaderVisibility::Vertex: return D3D12_SHADER_VISIBILITY_VERTEX;
				case ShaderVisibility::Pixel: return D3D12_SHADER_VISIBILITY_PIXEL;
				case ShaderVisibility::Geometry: return D3D12_SHADER_VISIBILITY_GEOMETRY;
				case ShaderVisibility::Domain: return D3D12_SHADER_VISIBILITY_DOMAIN;
				case ShaderVisibility::Hull: return D3D12_SHADER_VISIBILITY_HULL;
				default: return D3D12_SHADER_VISIBILITY_ALL;
				}
			};

		for (u32 i = 0; i < desc.numParameters; ++i)
		{
			ranges[i].RangeType = getRangeFunc(i);
			ranges[i].NumDescriptors = 1;
			ranges[i].BaseShaderRegister = desc.pParameters[i].registerIndex;
			ranges[i].RegisterSpace = 0;
			ranges[i].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			rootParameters[i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParameters[i].DescriptorTable.NumDescriptorRanges = 1;
			rootParameters[i].DescriptorTable.pDescriptorRanges = &ranges[i];
			rootParameters[i].ShaderVisibility = getShaderVisFunc(i);
		}

		D3D12_ROOT_SIGNATURE_DESC rd{};
		rd.NumParameters = desc.numParameters;
		rd.pParameters = rootParameters;
		rd.NumStaticSamplers = 0;
		rd.pStaticSamplers = nullptr;
		rd.Flags = desc.flags;

		ID3DBlob *pSignature{ nullptr };
		ID3DBlob *pError{ nullptr };
		auto hr = D3D12SerializeRootSignature(&rd, D3D_ROOT_SIGNATURE_VERSION_1, &pSignature, &pError);
		if (FAILED(hr))
		{
			ym::SafeRelease(pSignature);
			ym::SafeRelease(pError);
			return false;
		}

		hr = pDev->GetDeviceDep()->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(&pRootSignature_));
		ym::SafeRelease(pSignature);
		ym::SafeRelease(pError);
		if (FAILED(hr))
		{
			return false;
		}

		return true;
	}

	//----
	bool RootSignature::Init(Device *pDev, const D3D12_ROOT_SIGNATURE_DESC &desc)
	{
		ID3DBlob *blob = nullptr;
		ID3DBlob *error = nullptr;
		bool ret = true;

		auto hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error);
		if (FAILED(hr))
		{
			ret = false;
			goto D3D_ERROR;
		}

		hr = pDev->GetDeviceDep()->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&pRootSignature_));
		if (FAILED(hr))
		{
			ret = false;
			goto D3D_ERROR;
		}

	D3D_ERROR:
		ym::SafeRelease(blob);
		ym::SafeRelease(error);
		return ret;
	}

	//----
	bool RootSignature::Init(Device *pDev, const D3D12_VERSIONED_ROOT_SIGNATURE_DESC &desc)
	{
		ID3DBlob *blob = nullptr;
		ID3DBlob *error = nullptr;
		bool ret = true;

		auto hr = D3D12SerializeVersionedRootSignature(&desc, &blob, &error);
		if (FAILED(hr))
		{
			ret = false;
			goto D3D_ERROR;
		}

		hr = pDev->GetDeviceDep()->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&pRootSignature_));
		if (FAILED(hr))
		{
			ret = false;
			goto D3D_ERROR;
		}

	D3D_ERROR:
		ym::SafeRelease(blob);
		ym::SafeRelease(error);
		return ret;
	}

	//----
	bool RootSignature::Init(Device *pDev, Shader *vs, Shader *ps, Shader *gs, Shader *hs, Shader *ds)
	{
		/*
		D3D12_DESCRIPTOR_RANGE_TYPE RangeType;
		UINT NumDescriptors;
		UINT BaseShaderRegister;
		UINT RegisterSpace;
		UINT OffsetInDescriptorsFromTableStart;
		*/
		const D3D12_DESCRIPTOR_RANGE kDefaultRange[] = {
			{ D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 16, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
			{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 48, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
			{ D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 16, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
			{ D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 16, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
		};
		D3D12_DESCRIPTOR_RANGE ranges[16];
		D3D12_ROOT_PARAMETER params[16];
		int rangeCnt = 0;
		auto SetParam = [&](D3D12_SHADER_VISIBILITY vis)
			{
				params[rangeCnt].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				params[rangeCnt].ShaderVisibility = vis;
				params[rangeCnt].DescriptorTable.pDescriptorRanges = ranges + rangeCnt;
				params[rangeCnt].DescriptorTable.NumDescriptorRanges = 1;
			};
		D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

		if (vs)
		{
			inputIndex_.vsCbvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_VERTEX);
			ranges[rangeCnt++] = kDefaultRange[0];
			inputIndex_.vsSrvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_VERTEX);
			ranges[rangeCnt++] = kDefaultRange[1];
			inputIndex_.vsSamplerIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_VERTEX);
			ranges[rangeCnt++] = kDefaultRange[2];
			flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
		}
		if (ps)
		{
			inputIndex_.psCbvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_PIXEL);
			ranges[rangeCnt++] = kDefaultRange[0];
			inputIndex_.psSrvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_PIXEL);
			ranges[rangeCnt++] = kDefaultRange[1];
			inputIndex_.psSamplerIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_PIXEL);
			ranges[rangeCnt++] = kDefaultRange[2];
			inputIndex_.psUavIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_PIXEL);
			ranges[rangeCnt++] = kDefaultRange[3];
			flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
		}
		if (gs)
		{
			inputIndex_.gsCbvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_GEOMETRY);
			ranges[rangeCnt++] = kDefaultRange[0];
			inputIndex_.gsSrvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_GEOMETRY);
			ranges[rangeCnt++] = kDefaultRange[1];
			inputIndex_.gsSamplerIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_GEOMETRY);
			ranges[rangeCnt++] = kDefaultRange[2];
			flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
		}
		if (hs)
		{
			inputIndex_.hsCbvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_HULL);
			ranges[rangeCnt++] = kDefaultRange[0];
			inputIndex_.hsSrvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_HULL);
			ranges[rangeCnt++] = kDefaultRange[1];
			inputIndex_.hsSamplerIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_HULL);
			ranges[rangeCnt++] = kDefaultRange[2];
			flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
		}
		if (ds)
		{
			inputIndex_.dsCbvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_DOMAIN);
			ranges[rangeCnt++] = kDefaultRange[0];
			inputIndex_.dsSrvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_DOMAIN);
			ranges[rangeCnt++] = kDefaultRange[1];
			inputIndex_.dsSamplerIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_DOMAIN);
			ranges[rangeCnt++] = kDefaultRange[2];
			flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
		}

		D3D12_ROOT_SIGNATURE_DESC desc{};
		desc.NumParameters = rangeCnt;
		desc.pParameters = params;
		desc.NumStaticSamplers = 0;
		desc.pStaticSamplers = nullptr;
		desc.Flags = flags;

		return Init(pDev, desc);
	}

	//----
	bool RootSignature::Init(Device *pDev, Shader *as, Shader *ms, Shader *ps)
	{
		D3D12_DESCRIPTOR_RANGE_FLAGS range_flags = D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE;
		const D3D12_DESCRIPTOR_RANGE1 kDefaultRange[] = {
			{ D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 16, 0, 0, range_flags, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
			{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 48, 0, 0, range_flags, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
			{ D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 16, 0, 0, range_flags, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
			{ D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 16, 0, 0, range_flags, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
		};
		D3D12_DESCRIPTOR_RANGE1 ranges[16];
		D3D12_ROOT_PARAMETER1 params[16];
		int rangeCnt = 0;
		auto SetParam = [&](D3D12_SHADER_VISIBILITY vis)
			{
				params[rangeCnt].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				params[rangeCnt].ShaderVisibility = vis;
				params[rangeCnt].DescriptorTable.pDescriptorRanges = ranges + rangeCnt;
				params[rangeCnt].DescriptorTable.NumDescriptorRanges = 1;
			};
		D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

		if (as)
		{
			inputIndex_.asCbvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_AMPLIFICATION);
			ranges[rangeCnt++] = kDefaultRange[0];
			inputIndex_.asSrvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_AMPLIFICATION);
			ranges[rangeCnt++] = kDefaultRange[1];
			inputIndex_.asSamplerIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_AMPLIFICATION);
			ranges[rangeCnt++] = kDefaultRange[2];
			flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS;
		}
		if (ms)
		{
			inputIndex_.msCbvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_MESH);
			ranges[rangeCnt++] = kDefaultRange[0];
			inputIndex_.msSrvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_MESH);
			ranges[rangeCnt++] = kDefaultRange[1];
			inputIndex_.msSamplerIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_MESH);
			ranges[rangeCnt++] = kDefaultRange[2];
			flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS;
		}
		if (ps)
		{
			inputIndex_.psCbvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_PIXEL);
			ranges[rangeCnt++] = kDefaultRange[0];
			inputIndex_.psSrvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_PIXEL);
			ranges[rangeCnt++] = kDefaultRange[1];
			inputIndex_.psSamplerIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_PIXEL);
			ranges[rangeCnt++] = kDefaultRange[2];
			inputIndex_.psUavIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_PIXEL);
			ranges[rangeCnt++] = kDefaultRange[3];
			flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
		}

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC desc{};
		desc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
		desc.Desc_1_1.NumParameters = rangeCnt;
		desc.Desc_1_1.pParameters = params;
		desc.Desc_1_1.NumStaticSamplers = 0;
		desc.Desc_1_1.pStaticSamplers = nullptr;
		desc.Desc_1_1.Flags = flags;

		return Init(pDev, desc);
	}

	//----
	bool RootSignature::Init(Device *pDev, Shader *cs)
	{
		/*
		D3D12_DESCRIPTOR_RANGE_TYPE RangeType;
		UINT NumDescriptors;
		UINT BaseShaderRegister;
		UINT RegisterSpace;
		UINT OffsetInDescriptorsFromTableStart;
		*/
		const D3D12_DESCRIPTOR_RANGE kDefaultRange[] = {
			{ D3D12_DESCRIPTOR_RANGE_TYPE_CBV,     16, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
			{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV,     48, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
			{ D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 16, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
			{ D3D12_DESCRIPTOR_RANGE_TYPE_UAV,     16, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
		};
		D3D12_DESCRIPTOR_RANGE ranges[4];
		D3D12_ROOT_PARAMETER params[4];
		int rangeCnt = 0;
		auto SetParam = [&](D3D12_SHADER_VISIBILITY vis)
			{
				params[rangeCnt].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				params[rangeCnt].ShaderVisibility = vis;
				params[rangeCnt].DescriptorTable.pDescriptorRanges = ranges + rangeCnt;
				params[rangeCnt].DescriptorTable.NumDescriptorRanges = 1;
			};
		inputIndex_.csCbvIndex_ = rangeCnt;
		SetParam(D3D12_SHADER_VISIBILITY_ALL);
		ranges[rangeCnt++] = kDefaultRange[0];
		inputIndex_.csSrvIndex_ = rangeCnt;
		SetParam(D3D12_SHADER_VISIBILITY_ALL);
		ranges[rangeCnt++] = kDefaultRange[1];
		inputIndex_.csSamplerIndex_ = rangeCnt;
		SetParam(D3D12_SHADER_VISIBILITY_ALL);
		ranges[rangeCnt++] = kDefaultRange[2];
		inputIndex_.csUavIndex_ = rangeCnt;
		SetParam(D3D12_SHADER_VISIBILITY_ALL);
		ranges[rangeCnt++] = kDefaultRange[3];

		D3D12_ROOT_SIGNATURE_DESC desc{};
		desc.NumParameters = rangeCnt;
		desc.pParameters = params;
		desc.NumStaticSamplers = 0;
		desc.pStaticSamplers = nullptr;
		desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

		return Init(pDev, desc);
	}

	//----
	bool RootSignature::InitWithBindless(Device *pDev, Shader *vs, Shader *ps, Shader *gs, Shader *hs, Shader *ds, const RootBindlessInfo *bindlessInfos, u32 bindlessCount)
	{
		if (!bindlessInfos || !bindlessCount)
		{
			return Init(pDev, vs, ps, gs, hs, ds);
		}

		const D3D12_DESCRIPTOR_RANGE kDefaultRange[] = {
			{ D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 16, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
			{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 48, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
			{ D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 16, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
			{ D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 16, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
		};
		std::vector<D3D12_DESCRIPTOR_RANGE> ranges;
		std::vector<D3D12_ROOT_PARAMETER> params;
		ranges.resize(16 + (size_t)bindlessCount);
		params.resize(16 + (size_t)bindlessCount);
		int rangeCnt = 0;
		auto SetParam = [&](D3D12_SHADER_VISIBILITY vis)
			{
				params[rangeCnt].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				params[rangeCnt].ShaderVisibility = vis;
				params[rangeCnt].DescriptorTable.pDescriptorRanges = ranges.data() + rangeCnt;
				params[rangeCnt].DescriptorTable.NumDescriptorRanges = 1;
			};
		D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

		if (vs)
		{
			inputIndex_.vsCbvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_VERTEX);
			ranges[rangeCnt++] = kDefaultRange[0];
			inputIndex_.vsSrvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_VERTEX);
			ranges[rangeCnt++] = kDefaultRange[1];
			inputIndex_.vsSamplerIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_VERTEX);
			ranges[rangeCnt++] = kDefaultRange[2];
			flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
		}
		if (ps)
		{
			inputIndex_.psCbvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_PIXEL);
			ranges[rangeCnt++] = kDefaultRange[0];
			inputIndex_.psSrvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_PIXEL);
			ranges[rangeCnt++] = kDefaultRange[1];
			inputIndex_.psSamplerIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_PIXEL);
			ranges[rangeCnt++] = kDefaultRange[2];
			inputIndex_.psUavIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_PIXEL);
			ranges[rangeCnt++] = kDefaultRange[3];
			flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
		}
		if (gs)
		{
			inputIndex_.gsCbvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_GEOMETRY);
			ranges[rangeCnt++] = kDefaultRange[0];
			inputIndex_.gsSrvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_GEOMETRY);
			ranges[rangeCnt++] = kDefaultRange[1];
			inputIndex_.gsSamplerIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_GEOMETRY);
			ranges[rangeCnt++] = kDefaultRange[2];
			flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
		}
		if (hs)
		{
			inputIndex_.hsCbvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_HULL);
			ranges[rangeCnt++] = kDefaultRange[0];
			inputIndex_.hsSrvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_HULL);
			ranges[rangeCnt++] = kDefaultRange[1];
			inputIndex_.hsSamplerIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_HULL);
			ranges[rangeCnt++] = kDefaultRange[2];
			flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
		}
		if (ds)
		{
			inputIndex_.dsCbvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_DOMAIN);
			ranges[rangeCnt++] = kDefaultRange[0];
			inputIndex_.dsSrvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_DOMAIN);
			ranges[rangeCnt++] = kDefaultRange[1];
			inputIndex_.dsSamplerIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_DOMAIN);
			ranges[rangeCnt++] = kDefaultRange[2];
			flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
		}

		// bindless table.
		bindlessInfos_.reserve(bindlessCount);
		for (u32 i = 0; i < bindlessCount; i++)
		{
			RootBindlessInfo info = bindlessInfos[i];

			D3D12_DESCRIPTOR_RANGE range;
			range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			range.NumDescriptors = info.maxResources_;
			range.BaseShaderRegister = 0;
			range.RegisterSpace = info.space_;
			range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			info.index_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_PIXEL);
			ranges[rangeCnt++] = range;

			bindlessInfos_.push_back(info);
		}

		D3D12_ROOT_SIGNATURE_DESC desc{};
		desc.NumParameters = rangeCnt;
		desc.pParameters = params.data();
		desc.NumStaticSamplers = 0;
		desc.pStaticSamplers = nullptr;
		desc.Flags = flags;

		return Init(pDev, desc);
	}

	//----
	bool RootSignature::InitWithBindless(Device *pDev, Shader *as, Shader *ms, Shader *ps, const RootBindlessInfo *bindlessInfos, u32 bindlessCount)
	{
		if (!bindlessInfos || !bindlessCount)
		{
			return Init(pDev, as, ms, ps);
		}

		D3D12_DESCRIPTOR_RANGE_FLAGS range_flags = D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE;
		const D3D12_DESCRIPTOR_RANGE1 kDefaultRange[] = {
			{ D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 16, 0, 0, range_flags, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
			{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 48, 0, 0, range_flags, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
			{ D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 16, 0, 0, range_flags, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
			{ D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 16, 0, 0, range_flags, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
		};
		std::vector<D3D12_DESCRIPTOR_RANGE1> ranges;
		std::vector<D3D12_ROOT_PARAMETER1> params;
		ranges.resize(16 + (size_t)bindlessCount);
		params.resize(16 + (size_t)bindlessCount);
		int rangeCnt = 0;
		auto SetParam = [&](D3D12_SHADER_VISIBILITY vis)
			{
				params[rangeCnt].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				params[rangeCnt].ShaderVisibility = vis;
				params[rangeCnt].DescriptorTable.pDescriptorRanges = ranges.data() + rangeCnt;
				params[rangeCnt].DescriptorTable.NumDescriptorRanges = 1;
			};
		D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS
			| D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

		if (as)
		{
			inputIndex_.asCbvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_AMPLIFICATION);
			ranges[rangeCnt++] = kDefaultRange[0];
			inputIndex_.asSrvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_AMPLIFICATION);
			ranges[rangeCnt++] = kDefaultRange[1];
			inputIndex_.asSamplerIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_AMPLIFICATION);
			ranges[rangeCnt++] = kDefaultRange[2];
			flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS;
		}
		if (ms)
		{
			inputIndex_.msCbvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_MESH);
			ranges[rangeCnt++] = kDefaultRange[0];
			inputIndex_.msSrvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_MESH);
			ranges[rangeCnt++] = kDefaultRange[1];
			inputIndex_.msSamplerIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_MESH);
			ranges[rangeCnt++] = kDefaultRange[2];
			flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS;
		}
		if (ps)
		{
			inputIndex_.psCbvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_PIXEL);
			ranges[rangeCnt++] = kDefaultRange[0];
			inputIndex_.psSrvIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_PIXEL);
			ranges[rangeCnt++] = kDefaultRange[1];
			inputIndex_.psSamplerIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_PIXEL);
			ranges[rangeCnt++] = kDefaultRange[2];
			inputIndex_.psUavIndex_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_PIXEL);
			ranges[rangeCnt++] = kDefaultRange[3];
			flags &= ~D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
		}

		// bindless table.
		bindlessInfos_.reserve(bindlessCount);
		for (u32 i = 0; i < bindlessCount; i++)
		{
			RootBindlessInfo info = bindlessInfos[i];

			D3D12_DESCRIPTOR_RANGE1 range;
			range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			range.NumDescriptors = info.maxResources_;
			range.BaseShaderRegister = 0;
			range.RegisterSpace = info.space_;
			range.Flags = range_flags;
			range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			info.index_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_PIXEL);
			ranges[rangeCnt++] = range;

			bindlessInfos_.push_back(info);
		}

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC desc{};
		desc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
		desc.Desc_1_1.NumParameters = rangeCnt;
		desc.Desc_1_1.pParameters = params.data();
		desc.Desc_1_1.NumStaticSamplers = 0;
		desc.Desc_1_1.pStaticSamplers = nullptr;
		desc.Desc_1_1.Flags = flags;

		return Init(pDev, desc);
	}

	//----
	bool RootSignature::InitWithBindless(Device *pDev, Shader *cs, const RootBindlessInfo *bindlessInfos, u32 bindlessCount)
	{
		if (!bindlessInfos || !bindlessCount)
		{
			return Init(pDev, cs);
		}

		const D3D12_DESCRIPTOR_RANGE kDefaultRange[] = {
			{ D3D12_DESCRIPTOR_RANGE_TYPE_CBV,     16, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
			{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV,     48, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
			{ D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 16, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
			{ D3D12_DESCRIPTOR_RANGE_TYPE_UAV,     16, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
		};
		std::vector<D3D12_DESCRIPTOR_RANGE> ranges;
		std::vector<D3D12_ROOT_PARAMETER> params;
		ranges.resize(4 + (size_t)bindlessCount);
		params.resize(4 + (size_t)bindlessCount);
		int rangeCnt = 0;
		auto SetParam = [&](D3D12_SHADER_VISIBILITY vis)
			{
				params[rangeCnt].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				params[rangeCnt].ShaderVisibility = vis;
				params[rangeCnt].DescriptorTable.pDescriptorRanges = ranges.data() + rangeCnt;
				params[rangeCnt].DescriptorTable.NumDescriptorRanges = 1;
			};

		// default table.
		inputIndex_.csCbvIndex_ = rangeCnt;
		SetParam(D3D12_SHADER_VISIBILITY_ALL);
		ranges[rangeCnt++] = kDefaultRange[0];
		inputIndex_.csSrvIndex_ = rangeCnt;
		SetParam(D3D12_SHADER_VISIBILITY_ALL);
		ranges[rangeCnt++] = kDefaultRange[1];
		inputIndex_.csSamplerIndex_ = rangeCnt;
		SetParam(D3D12_SHADER_VISIBILITY_ALL);
		ranges[rangeCnt++] = kDefaultRange[2];
		inputIndex_.csUavIndex_ = rangeCnt;
		SetParam(D3D12_SHADER_VISIBILITY_ALL);
		ranges[rangeCnt++] = kDefaultRange[3];

		// bindless table.
		bindlessInfos_.reserve(bindlessCount);
		for (u32 i = 0; i < bindlessCount; i++)
		{
			RootBindlessInfo info = bindlessInfos[i];

			D3D12_DESCRIPTOR_RANGE range;
			range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			range.NumDescriptors = info.maxResources_;
			range.BaseShaderRegister = 0;
			range.RegisterSpace = info.space_;
			range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			info.index_ = rangeCnt;
			SetParam(D3D12_SHADER_VISIBILITY_ALL);
			ranges[rangeCnt++] = range;

			bindlessInfos_.push_back(info);
		}

		D3D12_ROOT_SIGNATURE_DESC desc{};
		desc.NumParameters = rangeCnt;
		desc.pParameters = params.data();
		desc.NumStaticSamplers = 0;
		desc.pStaticSamplers = nullptr;
		desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

		return Init(pDev, desc);
	}

	//----
	void RootSignature::Destroy()
	{
		SafeRelease(pRootSignature_);
	}

}	// namespace ym
