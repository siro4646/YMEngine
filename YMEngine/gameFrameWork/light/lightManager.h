#pragma once

#include "buffer/buffer.h"
#include "bufferView/bufferView.h"

#include "../components/pointLight/pointLight.h"

namespace ym
{
	struct PointLightSceneData
	{
		float num = 0;
		float padding[3];
	};

	struct PointLightData;

	class Buffer;
	class BufferView;
	class ConstantBufferView;
	//シングルトン
	class LightManager
	{
	public:
		static LightManager *Instance()
		{
			static LightManager instance;
			return &instance;
		}
		void Init();
		void Uninit()
		{


			pointLightBuffer.reset();
			pointLightBufferView.reset();
			//constantBuffer_->Unmap();
			constantBuffer_.reset();
			//constantBufferView_->Destroy();
			constantBufferView_.reset();
			pointLightDatas.clear();

		}
		void Update();
		void Draw();

		
		int AddPointLight(PointLightData pointLightData)
		{
			if (!isFirst)
			{
				//CreatePointLightData();
				//ダミーを消す
				pointLightDatas.clear();				
				isFirst = true;
			}
			pointLightDatas.push_back(pointLightData);
			pointLightSceneData.num = pointLightDatas.size();
			//UpdateBuffer();
			return pointLightSceneData.num - 1;
		}
		void SetPointLight(int index, PointLightData pointLightData)
		{
			pointLightDatas[index] = pointLightData;
			//UpdateBuffer();
		}
		void RemovePointLight(int index)
		{
			pointLightDatas.erase(pointLightDatas.begin() + index);
			pointLightSceneData.num = pointLightDatas.size();
			//UpdateBuffer();
		}
		//デスクリプターの場所教えるやつ
		inline D3D12_CPU_DESCRIPTOR_HANDLE GetPointLightCPUHandle()const
		{
			return pointLightBufferView->GetDescInfo().cpuHandle;
		}
		inline std::shared_ptr<Buffer> GetPointLightBuffer()
		{
			return pointLightBuffer;
		}

		inline std::shared_ptr<BufferView> GetPointLightBufferView()
		{
			return pointLightBufferView;
		}

		inline std::shared_ptr<ConstantBufferView> GetConstantBufferView()
		{
			return constantBufferView_;
		}




		LightManager() {}
		~LightManager() {
			Uninit();
		}

	private:
		bool isFirst = false;
		PointLightData dumyPointLightData;
		void CreatePointLightData();
		void CreateBuffer();
		void UpdateBuffer();

		float pointLightNum = 0;
		std::vector<PointLightData> pointLightDatas;

		std::shared_ptr<Buffer> pointLightBuffer;
		std::shared_ptr<BufferView> pointLightBufferView;

		PointLightSceneData *pPointLightSceneData;
		PointLightSceneData pointLightSceneData;
		std::shared_ptr<Buffer> constantBuffer_;
		std::shared_ptr<ConstantBufferView> constantBufferView_;

	};
}