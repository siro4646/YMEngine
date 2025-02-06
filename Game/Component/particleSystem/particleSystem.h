#pragma once

#include "gameFrameWork/components/component.h"

namespace ym
{
#define MAX_PARTICLE 1000
	class ParticleInfo
	{
		Vector3 position;
		Vector3 velocity;
		float lifeTime;
		bool isAlive;
		//float size;
	};

	class Buffer;
	class VertexBufferView;
	class IndexBufferView;


	class ParticleSystem : public Component
	{
	public:
		using Component::Component;

		void Init()override;
		void FixedUpdate() override;
		void Update()override;
		void Draw()override;
		void Uninit()override;


	private:

		void CreateBuffer();

		std::vector<std::shared_ptr<Buffer>>vertexBuffer;
		std::vector<std::shared_ptr<VertexBufferView>>vertexBufferView; 
		std::vector<std::shared_ptr<Buffer>>indexBuffer;
		std::vector<std::shared_ptr<IndexBufferView>>indexBufferView;

		//void Create

		ParticleInfo particles[MAX_PARTICLE];	
	};

}