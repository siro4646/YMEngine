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

	class Shader
	{
	public:
		Shader()
		{}
		~Shader()
		{
			Destroy();
		}

		bool Init(Device *pDev, ShaderType::Type type, const char *filename);
		bool Init(Device *pDev, ShaderType::Type type, const void *pData, size_t size);
		void Destroy();

		// getter
		const void *GetData() const { return pData_; }
		size_t GetSize() const { return size_; }
		ShaderType::Type GetShaderType() const { return shaderType_; }

	private:
		u8 *pData_{ nullptr };
		size_t				size_{ 0 };
		ShaderType::Type	shaderType_{ ShaderType::Max };
	};	// class Shader

}	// namespace ym

//	EOF
