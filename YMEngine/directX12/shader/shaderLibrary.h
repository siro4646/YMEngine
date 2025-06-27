#pragma once

namespace ym
{
	class Shader;
	class Device;

	class ShaderLibrary :public Singleton<ShaderLibrary>
	{
	public:

		// シェーダーを取得（同じ名前ならキャッシュされたものを返す）
		std::shared_ptr<Shader> GetOrLoadShader(const std::string &shaderName);

		// 明示的に登録（外部で作ったShaderを追加）
		void Register(const std::string &shaderName, std::shared_ptr<Shader> shader);

		// クリア（再読み込みなどのタイミングで）
		void Clear();

		void UnInit()
		{
			shaderMap_.clear();
			device_ = nullptr; // デバイスポインタをクリア
		}

		// 新規追加：初期化関数（フォルダ列挙）
		void Init();

		// 外部から一覧を取得したいとき用
		const std::vector<std::string> &GetShaderFolderNames() const { return shaderFolderNames_; }

	private:

		std::unordered_map<std::string, std::shared_ptr<Shader>> shaderMap_;
		std::vector<std::string> shaderFolderNames_; // ← 追加：shaderフォルダ配下のディレクトリ名リスト
		Device *device_ = nullptr; // デバイスへのポインタ
	};

}