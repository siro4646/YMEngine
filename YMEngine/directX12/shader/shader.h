#pragma once


namespace ym
{
	class Device;

	struct ShaderType
	{
		enum Type
		{
			Vertex,
			Pixel,
			Geometry,
			Domain,
			Hull,
			Compute,
			Mesh,
			Amplification,
			Library,

			Max
		};
	};	// struct ShaderType

	enum class ShaderVariableType {
		Float,
		Float2,
		Float3,
		Float4,
		Matrix,
		Int,
		Bool,
		Unknown
	};

	struct ShaderVariable {
		std::string name;
		ShaderVariableType type;
		uint32_t offset;      // cb���̃I�t�Z�b�g
		uint32_t size;        // �o�C�g��
	};
	struct ShaderConstantBuffer {
		std::string name;
		uint32_t size; // ���v�o�b�t�@�T�C�Y
		uint32_t bindPoint = 0;
		std::vector<ShaderVariable> variables;
		std::vector<uint8_t> cpuData; // CPU���ŕێ��iraw�o�C�g�j
	};
	struct ShaderTextureInfo
	{
		std::string name;
		D3D_SHADER_INPUT_TYPE type; // D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE �Ȃ�
		uint32_t slot; // t0, t1...�ɑΉ�
		uint32_t bindCount; // �o�C���h�J�E���g�i1�ȏ�j
	};

	struct ShaderReflectionInfo {
		std::vector<ShaderConstantBuffer> constantBuffers;
		std::vector<ShaderTextureInfo> textures; // �e�N�X�`�����
	};



	class Shader
	{
	public:
		Shader()
		{}
		~Shader()
		{
			Destroy();
		}

		ShaderReflectionInfo Reflect() const;

		bool Init(Device *pDev, ShaderType::Type type, const char *filename);
		bool Init(Device *pDev, ShaderType::Type type, const void *pData, size_t size);


		void Destroy();

		// getter
		const void *GetData() const { return pData_; }
		size_t GetSize() const { return size_; }
		ShaderType::Type GetShaderType() const { return shaderType_; }

		inline void SetShaderByteCode(D3D12_SHADER_BYTECODE data)
		{
			prv = data;
		}

		inline D3D12_SHADER_BYTECODE GetShaderByteCode() const
		{
			return prv;
		}

		string GetFileName() const { return fileName_; }

		string GetCsoFileName() const { return csoFileName_; }

		string GetShaderName() const { return shaderName_; }

	private:

		std::string fileName_{};
		std::string csoFileName_{};
		std::string shaderName_{};
		D3D12_SHADER_BYTECODE prv = {};
		u8 *pData_{ nullptr };
		size_t				size_{ 0 };
		ShaderType::Type	shaderType_{ ShaderType::Max };
	};	// class Shader

}	// namespace ym

//	EOF
