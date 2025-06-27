#pragma once

namespace ym
{
	class Shader;
	class Device;

	class ShaderLibrary :public Singleton<ShaderLibrary>
	{
	public:

		// �V�F�[�_�[���擾�i�������O�Ȃ�L���b�V�����ꂽ���̂�Ԃ��j
		std::shared_ptr<Shader> GetOrLoadShader(const std::string &shaderName);

		// �����I�ɓo�^�i�O���ō����Shader��ǉ��j
		void Register(const std::string &shaderName, std::shared_ptr<Shader> shader);

		// �N���A�i�ēǂݍ��݂Ȃǂ̃^�C�~���O�Łj
		void Clear();

		void UnInit()
		{
			shaderMap_.clear();
			device_ = nullptr; // �f�o�C�X�|�C���^���N���A
		}

		// �V�K�ǉ��F�������֐��i�t�H���_�񋓁j
		void Init();

		// �O������ꗗ���擾�������Ƃ��p
		const std::vector<std::string> &GetShaderFolderNames() const { return shaderFolderNames_; }

	private:

		std::unordered_map<std::string, std::shared_ptr<Shader>> shaderMap_;
		std::vector<std::string> shaderFolderNames_; // �� �ǉ��Fshader�t�H���_�z���̃f�B���N�g�������X�g
		Device *device_ = nullptr; // �f�o�C�X�ւ̃|�C���^
	};

}