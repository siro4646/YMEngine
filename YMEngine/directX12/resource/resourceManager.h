#pragma once
#include "texture/texture.h"

namespace ym
{

	class Texture;
	struct Mesh;

	class ResourceManager
	{
	public:
		static ResourceManager *Instance()
		{
			if (!instance)
				instance = new ResourceManager();

			return instance;
		}		
		void Uninit();

		inline void SetTexture(std::string fileName, ym::Texture data)
		{
			textureMap_[fileName] = data;
		}
		inline ym::Texture GetTexture(std::string fileName)
		{
			return textureMap_[fileName];
		}
		inline bool FindTexture(std::string fileName)
		{
			return textureMap_.find(fileName) != textureMap_.end();
		}

		inline void SetMesh(std::string fileName, std::vector<ym::Mesh> data)
		{
			meshMap_[fileName] = data;
		}
		inline std::vector<ym::Mesh> GetMesh(std::string fileName)
		{
			return meshMap_[fileName];
		}
		inline bool FindMesh(std::string fileName)
		{
			return meshMap_.find(fileName) != meshMap_.end();
		}



		

	private:
		static ResourceManager *instance;

		unordered_map<std::string, ym::Texture> textureMap_;

		unordered_map<std::string, std::vector<ym::Mesh>> meshMap_;
	};
}