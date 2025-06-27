#include "material2.h"
#include "shader/shaderLibrary.h"

#include "camera/cameraManager.h"
#include "camera/camera.h"
#include "device/device.h"
#include "Renderer/renderer.h"
#include "commandList/commandList.h"

#include "sampler/sampler.h"
#include "rootSignature/rootSignature.h"
#include "pipelineState/pipelineState.h"
#include "descriptorSet/descriptorSet.h"

#include "buffer/buffer.h"
#include "bufferView/bufferView.h"

#include "texture/texture.h"
#include "imgui/imguiRender.h"

namespace ym::material
{
	Device *Material::device_ = nullptr;
	CommandList *Material::cmdList_ = nullptr;

	void Material::Init()
	{
		Init("unlitVS", "unlitPS");
	}

	void Material::Init(const string &vsFileName, const string &psFileName)
	{
		// デバイスの取得
		if (!device_)
			device_ = Renderer::Instance()->GetDevice();
		if (!cmdList_)
			cmdList_ = Renderer::Instance()->GetGraphicCommandList();

		// シェーダーの読み込み
		vs_ = ShaderLibrary::Instance().GetOrLoadShader(vsFileName);
		ps_ = ShaderLibrary::Instance().GetOrLoadShader(psFileName);

		// CB/Texture初期化（内部でリフレクション）
		InitConstantBuffer();

		BaseRootSignature();
		BasePipelineState();
		BaseSampler();
		isinitialized_ = true;
	}


	void Material::Update()
	{
		if (!device_) return;
		if (!isinitialized_)
		{
			Init("unlitVS", "unlitPS");
		}

		auto *cmd = Renderer::Instance()->GetGraphicCommandList();
		for (auto &[cbName, cbInfo] : cbuffers_) {
			if (cbInfo.useExternalBuffer || !cbInfo.dirty) continue;

			cbInfo.buffer->UpdateBuffer(device_, cmd, cbInfo.cpuData.data(), cbInfo.cpuData.size());
			cbInfo.dirty = false;
		}
	}


	//---------------------------------------------------------------------------
	// ユーティリティ: 変数名をキーに「どの CB／オフセットにあるか」を検索
	//---------------------------------------------------------------------------
	static bool findVariableOffset(
		const ShaderReflectionInfo &refl,
		const std::string &varName,
		/*out*/std::string &outCBName,
		/*out*/uint32_t &outOffset)
	{
		for (auto &cb : refl.constantBuffers) {
			for (auto &v : cb.variables) {
				if (v.name == varName) {
					outCBName = cb.name;
					outOffset = v.offset;
					return true;
				}
			}
		}
		return false;
	}

	//---------------------------------------------------------------------------
	// 1) SetFloat
	//---------------------------------------------------------------------------
	void Material::SetFloat(const std::string &name, float value)
	{
		bool found = false;

		auto applyToCBs = [&](const ShaderReflectionInfo &info) {
			for (const auto &cb : info.constantBuffers) {
				for (const auto &var : cb.variables) {
					if (var.name == name) {
						auto it = cbuffers_.find(cb.name);
						if (it != cbuffers_.end() && !it->second.useExternalBuffer) {
							memcpy(it->second.cpuData.data() + var.offset, &value, sizeof(float));
							it->second.dirty = true;
							found = true;
						}
					}
				}
			}
			};

		applyToCBs(vsReflectionInfo_);
		applyToCBs(psReflectionInfo_);

		if (!found) {
			//ym::DebugLog("Material::SetFloat: variable '%s' not found\n", name.c_str());
		}
	}


	void Material::SetFloat2(const std::string &name, const DirectX::XMFLOAT2 &value)
	{
		bool found = false;

		auto applyToCBs = [&](const ShaderReflectionInfo &info) {
			for (const auto &cb : info.constantBuffers) {
				for (const auto &var : cb.variables) {
					if (var.name == name) {
						auto it = cbuffers_.find(cb.name);
						if (it != cbuffers_.end() && !it->second.useExternalBuffer) {
							memcpy(it->second.cpuData.data() + var.offset, &value, sizeof(DirectX::XMFLOAT2));
							it->second.dirty = true;
							found = true;
						}
					}
				}
			}
			};

		applyToCBs(vsReflectionInfo_);
		applyToCBs(psReflectionInfo_);

		if (!found) {
			//ym::DebugLog("Material::SetFloat: variable '%s' not found\n", name.c_str());
		}
	}

	void Material::SetFloat3(const std::string &name, const DirectX::XMFLOAT3 &value)
	{
		bool found = false;

		auto applyToCBs = [&](const ShaderReflectionInfo &info) {
			for (const auto &cb : info.constantBuffers) {
				for (const auto &var : cb.variables) {
					if (var.name == name) {
						auto it = cbuffers_.find(cb.name);
						if (it != cbuffers_.end() && !it->second.useExternalBuffer) {
							memcpy(it->second.cpuData.data() + var.offset, &value, sizeof(DirectX::XMFLOAT3));
							it->second.dirty = true;
							found = true;
						}
					}
				}
			}
			};

		applyToCBs(vsReflectionInfo_);
		applyToCBs(psReflectionInfo_);

		if (!found) {
			//ym::DebugLog("Material::SetFloat: variable '%s' not found\n", name.c_str());
		}
	}

	void Material::SetFloat4(const std::string &name, const DirectX::XMFLOAT4 &value)
	{
		bool found = false;

		auto applyToCBs = [&](const ShaderReflectionInfo &info) {
			for (const auto &cb : info.constantBuffers) {
				for (const auto &var : cb.variables) {
					if (var.name == name) {
						auto it = cbuffers_.find(cb.name);
						if (it != cbuffers_.end() && !it->second.useExternalBuffer) {
							memcpy(it->second.cpuData.data() + var.offset, &value, sizeof(DirectX::XMFLOAT4));
							it->second.dirty = true;
							found = true;
						}
					}
				}
			}
			};

		applyToCBs(vsReflectionInfo_);
		applyToCBs(psReflectionInfo_);

		if (!found) {
			//ym::DebugLog("Material::SetFloat: variable '%s' not found\n", name.c_str());
		}
	}

	//---------------------------------------------------------------------------
	// 2) SetMatrix   (column-major 4×4)
	//---------------------------------------------------------------------------
	void Material::SetMatrix(const std::string &name, const XMMATRIX &m)
	{
		bool found = false;

		auto applyToCBs = [&](const ShaderReflectionInfo &info) {
			for (const auto &cb : info.constantBuffers) {
				for (const auto &var : cb.variables) {
					if (var.name == name) {
						auto it = cbuffers_.find(cb.name);
						if (it != cbuffers_.end() && !it->second.useExternalBuffer) {
							memcpy(it->second.cpuData.data() + var.offset, &m, sizeof(XMMATRIX));
							it->second.dirty = true;
							found = true;
						}
					}
				}
			}
			};
		applyToCBs(vsReflectionInfo_);
		applyToCBs(psReflectionInfo_);

		if (!found) {
			//ym::DebugLog("Material::SetMatrix: variable '%s' not found\n", name.c_str());
		}


	}

	void Material::SetTexture(const std::string &name, std::shared_ptr<Texture> texture)
	{
		auto it = textures_.find(name);
		if (it != textures_.end()) {
			it->second.texture = texture;
			//テクスチャが変わったらビューを更新する
			if (texture) {
				it->second.textureView = std::make_shared<TextureView>();
				it->second.textureView->Init(device_, texture.get());
				it->second.isInitialized = true;
			}
			else {
				it->second.textureView.reset();
				it->second.isInitialized = false;
			}
		}
		else {
		}
	}

	void Material::SetTexture(const std::string &name, const std::string filePath)
	{
		auto it = textures_.find(name);
		if (it != textures_.end()) {
			it->second.texture = std::make_shared<Texture>();
			it->second.texture->LoadTexture(device_, cmdList_, filePath.c_str());
			it->second.textureView = std::make_shared<TextureView>();
			it->second.textureView->Init(device_, it->second.texture.get());
			it->second.isInitialized = true;
			textures_[name] = it->second; // 更新
		}
		else {
		}
	}

	void Material::ApplyConstantBuffer(DescriptorSet *ds)
	{
		cmdList_->GetCommandList()->SetPipelineState(pipelineState_->GetPSO());
		ds->Reset();
		auto isUsedInVS = [&](const std::string &name) {
			for (const auto &cb : vsReflectionInfo_.constantBuffers)
				if (cb.name == name) return true;
			return false;
			};

		auto isUsedInPS = [&](const std::string &name) {
			for (const auto &cb : psReflectionInfo_.constantBuffers)
				if (cb.name == name) return true;
			return false;
			};

		for (auto &[cbName, cbInfo] : cbuffers_) {
			const auto cpuHandle = cbInfo.useExternalBuffer
				? cbInfo.cpuAddress
				: cbInfo.cbv->GetDescInfo().cpuHandle;

			if (isUsedInVS(cbName)) {
				ds->SetVsCbv(cbInfo.slot, cpuHandle);
			}
			if (isUsedInPS(cbName)) {
				ds->SetPsCbv(cbInfo.slot, cpuHandle);
			}
		}
		bool useSampler = false;
		for (auto &[texName, texInfo] : textures_) {
			if (texInfo.isInitialized) {
				if (texInfo.textureView) {
					//ym::DebugLog("Material::ApplyConstantBuffer: texture '%s' is set to slot %u\n", texName.c_str(), texInfo.slot);
					ds->SetPsSrv(texInfo.slot, texInfo.textureView->GetDescInfo().cpuHandle);
					useSampler = true; // サンプラーを使うならば
				}
				else {
					//ym::DebugLog("Material::ApplyConstantBuffer: texture '%s' has no view\n", texName.c_str());
				}
			}
			else {
			}
		}

		if (useSampler)ds->SetPsSampler(0, sampler_->GetDescInfo().cpuHandle);
		cmdList_->SetGraphicsRootSignatureAndDescriptorSet(rootSignature_.get(), ds);

	}


	//---------------------------------------------------------------------------
	// 5) ImGui 可視化
	//---------------------------------------------------------------------------
	void Material::DrawImGui()
	{
		DrawConstantBuffersImGui();
		DrawTexturesImGui();
		DrawShaderSettingsImGui();		
	}

//------------------------------------------------------------
//  ConstantBuffer 可視化／編集
//------------------------------------------------------------
	void Material::DrawConstantBuffersImGui()
	{
		if (!ImGui::TreeNode("Material ConstantBuffers")) return;
		ImGui::PushID("MaterialConstantBuffers##");

		for (auto &[cbName, cbInfo] : cbuffers_)
		{
			const bool isShared = cbInfo.useExternalBuffer;

			if (!ImGui::TreeNode(cbName.c_str())) continue;
			ImGui::Text("slot = %u  (%s)", cbInfo.slot, isShared ? "shared" : "unique");

			if (!isShared)
			{
				/* VS/PS の両方から一致する CB を探索 */
				const ShaderReflectionInfo *refls[] = { &vsReflectionInfo_, &psReflectionInfo_ };
				for (const auto *refl : refls)
				{
					for (const auto &cb : refl->constantBuffers)
					{
						if (cb.name != cbName) continue;

						for (const auto &var : cb.variables)
						{
							uint8_t *pData = cbInfo.cpuData.data() + var.offset;
							const char *label = var.name.c_str();

							auto markDirty = [&] { cbInfo.dirty = true; };

							switch (var.type)
							{
							case ShaderVariableType::Float:
								if (ImGui::DragFloat(label, reinterpret_cast<float *>(pData), 0.01f))
									markDirty();
								break;
							case ShaderVariableType::Float2:
								if (ImGui::DragFloat2(label, reinterpret_cast<float *>(pData), 0.01f))
									markDirty();
								break;
							case ShaderVariableType::Float3:
								if (ImGui::DragFloat3(label, reinterpret_cast<float *>(pData), 0.01f))
									markDirty();
								break;
							case ShaderVariableType::Float4:
								if (ImGui::DragFloat4(label, reinterpret_cast<float *>(pData), 0.01f))
									markDirty();
								break;
							case ShaderVariableType::Matrix:
							{
								float *m = reinterpret_cast<float *>(pData);
								for (int r = 0; r < 4; ++r)
								{
									char rowLbl[64];
									std::snprintf(rowLbl, sizeof(rowLbl), "%s[%d]", label, r);
									if (ImGui::DragFloat4(rowLbl, m + r * 4, 0.01f))
										markDirty();
								}
							}
							break;
							default:
								ImGui::TextDisabled("%s (unsupported)", label);
								break;
							}
						}
					}
				}
			}
			ImGui::TreePop(); // cbName
		}
		ImGui::PopID();     // MaterialConstantBuffers
		ImGui::TreePop();     // Material ConstantBuffers
	}
//------------------------------------------------------------
//  Texture スロット確認＆ロード
//------------------------------------------------------------
	void Material::DrawTexturesImGui()
	{
		if (!ImGui::TreeNode("Material Textures (SRV)")) return;

		/* ドラッグ＆ドロップ用の静的バッファ */
		static std::unordered_map<std::string, std::string> inputPaths;

		for (auto &[name, texInfo] : textures_)
		{
			ImGui::Separator();
			ImGui::Text("Name: %s", name.c_str());
			ImGui::Text("TextureName: %s", texInfo.texture ? texInfo.texture->GetTextureName().c_str() : "None");
			ImGui::Text("Slot: %u", texInfo.slot);
			ImGui::Text("Initialized: %s", texInfo.isInitialized ? "true" : "false");
			ImGuiRender::Instance()->DrawImage(*texInfo.textureView.get(), ImVec2(100, 100));

			// バッファ確保
			std::string &pathBuf = inputPaths[name];
			pathBuf.resize(std::max<size_t>(pathBuf.size(), 256));
			char *buffer = pathBuf.data();

			ImGui::PushID(name.c_str());

			ImGui::InputText("Path", buffer, pathBuf.size());

			// ドロップボタン
			std::vector<std::string> dropped;
			if (ImGuiRender::Instance()->DrawDropButton(&dropped, true) && !dropped.empty())
				std::strncpy(buffer, dropped[0].c_str(), pathBuf.size() - 1);

			ImGui::SameLine();
			if (ImGui::Button("Load"))
				SetTexture(name, std::string(buffer));

			ImGui::PopID();
		}
		ImGui::TreePop();
	}

//------------------------------------------------------------
//  VS / PS 選択 & リロード
//------------------------------------------------------------
	void Material::DrawShaderSettingsImGui()
	{
		if (!ImGui::TreeNode("Shader Settings")) return;

		auto &shaderFolders = ym::ShaderLibrary::Instance().GetShaderFolderNames();
		if (shaderFolders.empty())
		{
			ImGui::TextDisabled("No shader folders found.");
			ImGui::TreePop();
			return;
		}

		static int selVS = 0, selPS = 0;

		ImGui::Text("Current VS: %s", vs_ ? vs_->GetShaderName().c_str() : "None");
		ImGui::Text("Current PS: %s", ps_ ? ps_->GetShaderName().c_str() : "None");
		ImGui::Separator();

		auto combo = [&](const char *label, int &idx)
			{
				if (ImGui::BeginCombo(label, shaderFolders[idx].c_str()))
				{
					for (int i = 0; i < shaderFolders.size(); ++i)
					{
						bool selected = (idx == i);
						if (ImGui::Selectable(shaderFolders[i].c_str(), selected))
							idx = i;
						if (selected) ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
			};
		combo("Vertex Shader", selVS);
		combo("Pixel Shader", selPS);

		if (ImGui::Button("Reload Shader"))
		{
			const std::string vsName = shaderFolders[selVS] + "VS";
			const std::string psName = shaderFolders[selPS] + "PS";

			vs_ = ym::ShaderLibrary::Instance().GetOrLoadShader(vsName);
			ps_ = ym::ShaderLibrary::Instance().GetOrLoadShader(psName);

			InitConstantBuffer();   // CB / テクスチャ再構築
			BaseRootSignature();    // RS 再生成
			BasePipelineState();    // PSO 再生成
		}

		ImGui::TreePop();
	}


	void Material::InitConstantBuffer()
	{
		UnInitBuffer();
		DebugLog("aaaa");
		vsReflectionInfo_ = vs_->Reflect();
		for (const auto &cb : vsReflectionInfo_.constantBuffers)
		{
			MaterialConstantBufferInfo cbInfo{};
			cbInfo.slot = cb.bindPoint;

			if (cb.name == "SceneData") {
				cbInfo.useExternalBuffer = true;
				cbInfo.cpuAddress = CameraManager::Instance().GetMainCamera()->GetDescriptorHandle();
			}
			else {
				cbInfo.buffer = std::make_shared<Buffer>();
				cbInfo.buffer->Init(device_, cb.size, 256, BufferUsage::ConstantBuffer, true, false);
				cbInfo.cbv = std::make_shared<ConstantBufferView>();
				cbInfo.cbv->Init(device_, cbInfo.buffer.get());
				cbInfo.cpuData.resize(cb.size);
			}

			cbuffers_[cb.name] = std::move(cbInfo);
		}

		psReflectionInfo_ = ps_->Reflect();
		for (const auto &cb : psReflectionInfo_.constantBuffers)
		{
			if (cbuffers_.find(cb.name) != cbuffers_.end()) continue;

			MaterialConstantBufferInfo cbInfo{};
			cbInfo.slot = cb.bindPoint;

			if (cb.name == "SceneData") {
				cbInfo.useExternalBuffer = true;
				cbInfo.cpuAddress = CameraManager::Instance().GetMainCamera()->GetDescriptorHandle();
			}
			else {
				cbInfo.buffer = std::make_shared<Buffer>();
				cbInfo.buffer->Init(device_, cb.size, 256, BufferUsage::ConstantBuffer, true, false);
				cbInfo.cbv = std::make_shared<ConstantBufferView>();
				cbInfo.cbv->Init(device_, cbInfo.buffer.get());
				cbInfo.cpuData.resize(cb.size);
			}

			cbuffers_[cb.name] = std::move(cbInfo);
		}

		// PSシェーダー内のテクスチャスロット情報を反映
		for (const auto &tex : psReflectionInfo_.textures) {
			if (tex.type == D3D_SIT_TEXTURE)
			{
					MaterialTextureInfo texInfo{};
					texInfo.slot = tex.slot;
					texInfo.isInitialized = true;
					texInfo.texture = std::make_shared<Texture>();
					texInfo.texture->LoadTexture(device_, cmdList_, "n"); // 仮ロード
					texInfo.textureView = std::make_shared<TextureView>();
					texInfo.textureView->Init(device_, texInfo.texture.get());
					textures_[tex.name] = texInfo;
				
			}
		}
	}


	void Material::UnInitBuffer()
	{
		for (auto &[name, cbInfo] : cbuffers_) {
			if (cbInfo.buffer) {
				cbInfo.buffer->Unmap(); // マップ解除
				cbInfo.buffer->Destroy();
				cbInfo.buffer.reset();
			}
			if (cbInfo.cbv) {
				cbInfo.cbv->Destroy();
				cbInfo.cbv.reset(); // マップ解除
			}
		}
		cbuffers_.clear();

		for (auto &[name, texInfo] : textures_) {
			if (texInfo.texture) {
				texInfo.texture->Uninit(); // マップ解除
				texInfo.texture.reset();
			}
			if (texInfo.textureView) {
				texInfo.textureView->Destroy();
				texInfo.textureView.reset();
			}
		}
		textures_.clear();
	}

	void Material::BasePipelineState()
	{
		auto bbidx = device_->GetSwapChain().GetFrameIndex();
		auto &swapChain = device_->GetSwapChain();

		ym::GraphicsPipelineStateDesc desc;
		desc = GraphicsPipelineState::GetDefaultDesc();
		desc.rasterizer.cullMode = D3D12_CULL_MODE_NONE;
		desc.pRootSignature = rootSignature_.get();
		desc.pVS = vs_.get();
		desc.pPS = ps_.get();
		pipelineState_ = std::make_shared<GraphicsPipelineState>();
		pipelineState_->Init(device_, desc);
	}
	void Material::BaseRootSignature()
	{
		rootSignature_ = std::make_shared<RootSignature>();
		rootSignature_->Init(device_, vs_.get(), ps_.get(), nullptr, nullptr, nullptr);
	}
	void Material::BaseSampler()
	{
		D3D12_SAMPLER_DESC desc{};
		desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler_ = std::make_shared<Sampler>();
		sampler_->Initialize(device_, desc);
	}

} // namespace ym::material