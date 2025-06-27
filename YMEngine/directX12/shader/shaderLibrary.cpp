#include "shaderLibrary.h"
#include "shader.h"
#include "device/device.h"
#include "Renderer/renderer.h"

#include <filesystem>

namespace ym
{
	//シェーダーの名前+タイプで渡される ex) pbrVSみたいな感じ
	std::shared_ptr<Shader> ShaderLibrary::GetOrLoadShader(const std::string &shaderName)
	{
		if (!device_)
		{
			device_ = Renderer::Instance()->GetDevice(); // デバイスが未設定ならRendererから取得
		}
		auto it = shaderMap_.find(shaderName);
		if (it != shaderMap_.end())
		{
			return it->second;
		}

		// 存在しない場合は新しく読み込み
		auto shader = std::make_shared<Shader>();

		// サフィックスをもとにタイプ判定
		std::string suffix;
		if (shaderName.length() >= 2)
		{
			suffix = shaderName.substr(shaderName.length() - 2); // 末尾2文字を取得（VS, PSなど）
		}

		ShaderType::Type type;

		if (suffix == "VS")
			type = ShaderType::Vertex;
		else if (suffix == "PS")
			type = ShaderType::Pixel;
		else if (suffix == "GS")
			type = ShaderType::Geometry;
		else if (suffix == "HS")
			type = ShaderType::Hull;
		else if (suffix == "DS")
			type = ShaderType::Domain;
		else if (suffix == "CS")
			type = ShaderType::Compute;

		string baseName = shaderName.substr(0, shaderName.length() - 2); // サフィックスを除いた名前

		//読み込み
		if(!shader->Init(device_, type, baseName.c_str()))// .cso拡張子つける想定
		{
			DebugLog("ShaderLibrary: Failed to load shader %s\n", shaderName.c_str());
			return nullptr;
		}
		shaderMap_[shaderName] = shader;
		return shader;
	}

	void ShaderLibrary::Register(const std::string &shaderName, std::shared_ptr<Shader> shader)
	{
		shaderMap_[shaderName] = shader; // 明示的に登録
	}

	void ShaderLibrary::Clear()
	{
		shaderMap_.clear(); // 全てのシェーダーをクリア
	}
	void ShaderLibrary::Init()
	{
		shaderFolderNames_.clear();

		// shaderディレクトリのルート（パスは環境に応じて調整）
		const std::string shaderRoot = "asset/shader";

		try {
			for (const auto &entry : std::filesystem::directory_iterator(shaderRoot)) {
				if (entry.is_directory()) {
					std::string folderName = entry.path().filename().string();
					shaderFolderNames_.push_back(folderName);
				}
			}
		}
		catch (const std::exception &e) {
			DebugLog("ShaderLibrary::Init() - Failed to enumerate shader folders: %s\n", e.what());
		}
	}

}