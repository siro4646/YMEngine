
#include "shader.h"
#include "device/device.h"
#include "utility/fileStream/fileStream.h"

namespace ym
{
	//----
	bool Shader::Init(Device *pDev, ShaderType::Type type, const char *filename)
	{
		//�V�F�[�_�[�������w�肵�Ă���̃p�X����� ex) "simple"��type Vertex���n���ꂽ�� /asset/shader/simple/cso/simpleVS.cso��Ԃ�
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

		// �t�@�C����ǂݍ���
		File f;
		if (!f.ReadFile(path.c_str()))
		{
			return false;
		}

		// ������
		return Init(pDev, type, f.GetData(), f.GetSize());
	}

	//----
	bool Shader::Init(Device *pDev, ShaderType::Type type, const void *pData, size_t size)
	{
		if (!pData || !size)
		{
			return false;
		}

		// ���������m��
		pData_ = new u8[size];
		if (!pData_)
		{
			return false;
		}

		// �R�s�[
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