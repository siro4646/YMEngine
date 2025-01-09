#pragma once
#include "texture/texture.h"

namespace ym
{

	class Texture;

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



		

	private:
		static ResourceManager *instance;

		unordered_map<std::string, ym::Texture> textureMap_;
	};
}