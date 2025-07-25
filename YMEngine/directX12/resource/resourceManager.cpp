
#include "resourceManager.h"

namespace ym
{
	ResourceManager *ResourceManager::instance=nullptr;

	void ResourceManager::Uninit()
	{
		//要素があるなら
		ym::ConsoleLog("ResourceManager::Uninit()\n");
		ym::ConsoleLog("TextureMap Size : %d\n", textureMap_.size());

		if (!textureMap_.empty())
			textureMap_.clear();

		ym::ConsoleLog("MeshMap Size : %d\n", meshMap_.size());
		if (!meshMap_.empty())
			meshMap_.clear();
		
		delete instance;
		instance = nullptr;
	}
}
