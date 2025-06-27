#include "shaderLibrary.h"
#include "shader.h"
#include "device/device.h"
#include "Renderer/renderer.h"

#include <filesystem>

namespace ym
{
	//�V�F�[�_�[�̖��O+�^�C�v�œn����� ex) pbrVS�݂����Ȋ���
	std::shared_ptr<Shader> ShaderLibrary::GetOrLoadShader(const std::string &shaderName)
	{
		if (!device_)
		{
			device_ = Renderer::Instance()->GetDevice(); // �f�o�C�X�����ݒ�Ȃ�Renderer����擾
		}
		auto it = shaderMap_.find(shaderName);
		if (it != shaderMap_.end())
		{
			return it->second;
		}

		// ���݂��Ȃ��ꍇ�͐V�����ǂݍ���
		auto shader = std::make_shared<Shader>();

		// �T�t�B�b�N�X�����ƂɃ^�C�v����
		std::string suffix;
		if (shaderName.length() >= 2)
		{
			suffix = shaderName.substr(shaderName.length() - 2); // ����2�������擾�iVS, PS�Ȃǁj
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

		string baseName = shaderName.substr(0, shaderName.length() - 2); // �T�t�B�b�N�X�����������O

		//�ǂݍ���
		if(!shader->Init(device_, type, baseName.c_str()))// .cso�g���q����z��
		{
			DebugLog("ShaderLibrary: Failed to load shader %s\n", shaderName.c_str());
			return nullptr;
		}
		shaderMap_[shaderName] = shader;
		return shader;
	}

	void ShaderLibrary::Register(const std::string &shaderName, std::shared_ptr<Shader> shader)
	{
		shaderMap_[shaderName] = shader; // �����I�ɓo�^
	}

	void ShaderLibrary::Clear()
	{
		shaderMap_.clear(); // �S�ẴV�F�[�_�[���N���A
	}
	void ShaderLibrary::Init()
	{
		shaderFolderNames_.clear();

		// shader�f�B���N�g���̃��[�g�i�p�X�͊��ɉ����Ē����j
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