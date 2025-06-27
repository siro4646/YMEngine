#pragma once

#include "gameFrameWork/gameObject/gameObject.h"


namespace ym {
	enum class Direction
	{
		UP,
		DOWN,
		RIGHT,
		LEFT,
		FRONT,
		BACK,
		MAX
	};

	class Camera;
	class RenderTexture;
	//�R�s�[�p�e���v���[�g
	class CubeMapGenerater
	{
	public:
		void Generate(Vector3 position);
		void Uninit();
		void Draw();
	private:
		void Init();
		


	private:
		std::shared_ptr<RenderTexture> renderTexture[(int)Direction::MAX];
		Camera *camera[6]{};
		Transform transform;
		bool isInit = false;
	};
}