
#include "shader.h"
#include "device/device.h"
#include "utility/fileStream/fileStream.h"
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler")
#define DXIL_FOURCC(ch0, ch1, ch2, ch3) \
    ((uint32_t)(uint8_t)(ch0) | ((uint32_t)(uint8_t)(ch1) << 8) | \
    ((uint32_t)(uint8_t)(ch2) << 16) | ((uint32_t)(uint8_t)(ch3) << 24))


namespace ym
{
	ShaderReflectionInfo Shader::Reflect() const {
		ShaderReflectionInfo info;

		// シェーダーバイナリファイル読み込み
		std::ifstream infile(csoFileName_, std::ios::binary);
		if (!infile) {
			ym::DebugLog("[Shader::Reflect] Failed to open shader binary: %s", csoFileName_.c_str());
			return info;
		}

		std::vector<char> data;
		infile.seekg(0, std::ios::end);
		data.resize(infile.tellg());
		infile.seekg(0, std::ios::beg);
		infile.read(data.data(), data.size());

		// DXC API 初期化
		ComPtr<IDxcLibrary> lib;
		DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&lib));

		ComPtr<IDxcBlobEncoding> binBlob;
		lib->CreateBlobWithEncodingOnHeapCopy(data.data(), data.size(), CP_ACP, &binBlob);

		ComPtr<IDxcContainerReflection> refl;
		DxcCreateInstance(CLSID_DxcContainerReflection, IID_PPV_ARGS(&refl));
		refl->Load(binBlob.Get());

		UINT index = 0;
		if (FAILED(refl->FindFirstPartKind(DXIL_FOURCC('D', 'X', 'I', 'L'), &index))) {
			ym::DebugLog("[Shader::Reflect] DXIL chunk not found.");
			return info;
		}

		ComPtr<ID3D12ShaderReflection> shaderReflection;
		if (FAILED(refl->GetPartReflection(index, IID_PPV_ARGS(&shaderReflection)))) {
			ym::DebugLog("[Shader::Reflect] Failed to get shader reflection.");
			return info;
		}

		D3D12_SHADER_DESC shaderDesc{};
		if (FAILED(shaderReflection->GetDesc(&shaderDesc))) {
			ym::DebugLog("[Shader::Reflect] Failed to get shader desc.");
			return info;
		}
		ym::DebugLog("Shader Desc:");
		ym::DebugLog("  Creator: %s", shaderDesc.Creator);
		ym::DebugLog("  Version: %u", shaderDesc.Version);
		ym::DebugLog("  ConstantBuffers: %u", shaderDesc.ConstantBuffers);
		ym::DebugLog("  BoundResources: %u", shaderDesc.BoundResources);
		ym::DebugLog("  InputParameters: %u", shaderDesc.InputParameters);
		ym::DebugLog("  OutputParameters: %u", shaderDesc.OutputParameters);

		for (UINT i = 0; i < shaderDesc.BoundResources; ++i) {
			D3D12_SHADER_INPUT_BIND_DESC bindDesc{};
			if (SUCCEEDED(shaderReflection->GetResourceBindingDesc(i, &bindDesc))) {
				ym::DebugLog("  [%u] %s - Type: %d, BindPoint: %u, BindCount: %u",
					i,
					bindDesc.Name,
					bindDesc.Type,
					bindDesc.BindPoint,
					bindDesc.BindCount);
				ShaderTextureInfo texInfo;
				texInfo.name = bindDesc.Name;
				texInfo.type = static_cast<D3D_SHADER_INPUT_TYPE>(bindDesc.Type);
				texInfo.slot = bindDesc.BindPoint;
				texInfo.bindCount = bindDesc.BindCount;
				info.textures.push_back(texInfo);
			}
		}



		for (UINT i = 0; i < shaderDesc.ConstantBuffers; ++i) {
			auto *cb = shaderReflection->GetConstantBufferByIndex(i);
			D3D12_SHADER_BUFFER_DESC cbDesc{};
			if (FAILED(cb->GetDesc(&cbDesc))) continue;

			ShaderConstantBuffer scb;
			DebugLog("%s: Size=%u, Variables=%u", cbDesc.Name, cbDesc.Size, cbDesc.Variables);
			scb.name = cbDesc.Name;
			scb.size = cbDesc.Size;
			scb.cpuData.resize(cbDesc.Size);

			// バインドスロット番号を取得
			D3D12_SHADER_INPUT_BIND_DESC bindDesc{};
			if (SUCCEEDED(shaderReflection->GetResourceBindingDescByName(cbDesc.Name, &bindDesc))) {
				scb.bindPoint = bindDesc.BindPoint;
			}
			else {
				scb.bindPoint = 0xFFFFFFFF; // バインド情報取得失敗時
				ym::DebugLog("[Shader::Reflect] Failed to get bind point for CB: %s", cbDesc.Name);
			}
			for (UINT v = 0; v < cbDesc.Variables; ++v) {
				auto *var = cb->GetVariableByIndex(v);
				D3D12_SHADER_VARIABLE_DESC varDesc{};
				if (FAILED(var->GetDesc(&varDesc))) continue;

				auto *type = var->GetType();
				D3D12_SHADER_TYPE_DESC typeDesc{};
				if (FAILED(type->GetDesc(&typeDesc))) continue;

				ShaderVariable sv;
				sv.name = varDesc.Name;
				sv.offset = varDesc.StartOffset;
				sv.size = varDesc.Size;

				// 型判定（簡略化）
				if (typeDesc.Type == D3D_SVT_FLOAT) {
					if (typeDesc.Class == D3D_SVC_SCALAR) sv.type = ShaderVariableType::Float;
					else if (typeDesc.Class == D3D_SVC_VECTOR) {
						switch (typeDesc.Columns) {
						case 2: sv.type = ShaderVariableType::Float2; break;
						case 3: sv.type = ShaderVariableType::Float3; break;
						case 4: sv.type = ShaderVariableType::Float4; break;
						}
					}
					else if (typeDesc.Class == D3D_SVC_MATRIX_COLUMNS)
						sv.type = ShaderVariableType::Matrix;
				}
				else if (typeDesc.Type == D3D_SVT_INT)
					sv.type = ShaderVariableType::Int;
				else if (typeDesc.Type == D3D_SVT_BOOL)
					sv.type = ShaderVariableType::Bool;
				else
					sv.type = ShaderVariableType::Unknown;

				scb.variables.push_back(sv);
			}

			info.constantBuffers.push_back(scb);
		}

		ym::DebugLog("[Shader::Reflect] Reflection complete. Total CBs: %zu", info.constantBuffers.size());
		return info;
	}





	//----
	bool Shader::Init(Device *pDev, ShaderType::Type type, const char *filename)
	{
		//シェーダー名だけ指定してそれのパスを作る ex) "simple"とtype Vertexが渡されたら /asset/shader/simple/cso/simpleVS.csoを返す
		std::string path = "asset/shader/";
		path += filename;
		path += "/cso/";
		path += filename;
		switch (type)
		{
		case ShaderType::Type::Vertex:
			path += "VS.cso";
			break;
		case ShaderType::Type::Pixel:
			path += "PS.cso";
			break;
		case ShaderType::Type::Geometry:
			path += "GS.cso";
			break;
		case ShaderType::Type::Hull:
			path += "HS.cso";
			break;
		case ShaderType::Type::Domain:
			path += "DS.cso";
			break;
		case ShaderType::Type::Compute:
			path += "CS.cso";
			break;
		}

		// ファイルを読み込む
		File f;
		if (!f.ReadFile(path.c_str()))
		{
			return false;
		}
		std::string path2 = "asset/shader/";
		path2 += filename;
		path2 += "/";
		path2 += filename;

		switch (type)
		{
		case ShaderType::Type::Vertex:
			path2 += "VS.hlsl";
			break;
		case ShaderType::Type::Pixel:
			path2 += "PS.hlsl";
			break;
		case ShaderType::Type::Geometry:
			path2 += "GS.hlsl";
			break;
		case ShaderType::Type::Hull:
			path2 += "HS.hlsl";
			break;
		case ShaderType::Type::Domain:
			path2 += "DS.hlsl";
			break;
		case ShaderType::Type::Compute:
			path2 += "CS.hlsl";
			break;
		}

		fileName_ = path2;
		csoFileName_ = path;
		shaderName_ = filename;
		// 初期化
		return Init(pDev, type, f.GetData(), f.GetSize());
	}

	//----
	bool Shader::Init(Device *pDev, ShaderType::Type type, const void *pData, size_t size)
	{
		if (!pData || !size)
		{
			return false;
		}

		// メモリを確保
		pData_ = new u8[size];
		if (!pData_)
		{
			return false;
		}

		// コピー
		memcpy(pData_, pData, size);

		size_ = size;
		shaderType_ = type;

		return true;
	}

	//----
	void Shader::Destroy()
	{
		ym::SafeDeleteArray(pData_);
	}

}	// namespace ym