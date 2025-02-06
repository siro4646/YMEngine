#pragma once
#include <windows.h>
//Xaudio2
#include <XAudio2.h>
#include <X3DAudio.h>
#pragma comment(lib, "xaudio2.lib")

//Mp3
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mfobjects.h>
#include <iostream>
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")

#pragma comment(lib, "mfuuid.lib")
//ogg

//#pragma comment(lib, "vorbisfile.lib")

#include <cstdio>
#include <cmath>

#include <DirectXMath.h>




#include <string>
#include <locale>
#include <codecvt>

#include <map>
#include <unordered_map>
#include <vector>

namespace Utility
{
	namespace Sound
	{
		bool LoadWaveFile(const char *filename, WAVEFORMATEX &wf, XAUDIO2_BUFFER &buffer, BYTE *&waveData);
		bool LoadMP3File(const wchar_t *filename, WAVEFORMATEX &wf, XAUDIO2_BUFFER &buffer, BYTE *&waveData);
	}
}
namespace System
{
	namespace Config
	{
		namespace Sound
		{
			extern const unsigned short BgmMax_2D;
			extern const unsigned short SeMax_2D;
			extern const unsigned short BgmMax_3D;
			extern const unsigned short SeMax_3D;

		}
	}
}

namespace ym {
	class Sound
	{
	public:
		enum Tag
		{
			Any,
			Title,
			Game,
			Result,
		};
		enum class PlayType {
			BGM2D,
			SE2D,
			BGM3D,
			SE3D,
		};
		Sound() {

		}

		Sound(std::string name, std::string flinename, Tag tag, float vol)
		{
			m_name = name;
			m_fileName = flinename;
			m_tag = tag;
			m_vol = vol;

		}

		~Sound() {}


	private:
		std::string m_name;
		std::string m_fileName;
		Tag m_tag = Tag::Any;
		float m_vol = 1.0f;

		bool isLoaded = false;

		WAVEFORMATEX wf{};
		XAUDIO2_BUFFER buffer{};
		BYTE *waveData{};

	public:

		bool Load(const char *name, const char *filename);
		bool Load(std::string name, std::string filename);
		bool LoadMP3(std::string name, std::string filename);
		XAUDIO2_BUFFER &GetBuffer() { return buffer; }

		DWORD GetChannelCount() { return wf.nChannels; }

		std::string GetName() { return m_name; }
		std::string GetFileName() { return m_fileName; }
		Tag GetTag() { return m_tag; }
		float GetVol() { return m_vol; }
		void SetVol(float vol) { m_vol = vol; }
		bool IsLoaded() { return isLoaded; }
	};
	class SourceData3D
	{
	private:
	public:

		IXAudio2SourceVoice *m_SrcVoice = nullptr;
		X3DAUDIO_EMITTER m_Emitter = { 0 };

		DirectX::XMFLOAT3 *m_pos = nullptr;
		DirectX::XMFLOAT3 m_initPos = { 0.0f, 0.0f, 0.0f };

		X3DAUDIO_DSP_SETTINGS m_DSPSettings = { 0 };
		IXAudio2SourceVoice *GetSrcVoice() { return m_SrcVoice; }
		X3DAUDIO_EMITTER &GetEmitter() { return m_Emitter; }
		X3DAUDIO_DSP_SETTINGS &GetDSPSettings() { return m_DSPSettings; }

		void SetSrcVoice(IXAudio2SourceVoice *srcVoice) { m_SrcVoice = srcVoice; }
		void SetEmitter(X3DAUDIO_EMITTER emitter) { m_Emitter = emitter; }
		void SetDSPSettings(X3DAUDIO_DSP_SETTINGS dspSettings) { m_DSPSettings = dspSettings; }
	};


	//シングルトン
	class SoundManager
	{
	private:
		//xaudio2
		IXAudio2 *m_XAudio2 = nullptr;
		IXAudio2MasteringVoice *m_MasterVoice = nullptr;
		XAUDIO2_VOICE_DETAILS m_voiceDetails;
		std::vector<IXAudio2SourceVoice *> m_SrcBgm2D{};
		std::vector<IXAudio2SourceVoice *> m_SrcSe2D{};
		std::vector<SourceData3D> m_SrcBgm3D{};
		std::vector<SourceData3D> m_SrcSe3D{};

		DWORD m_dwChannelMask;

		//x3daudio
		X3DAUDIO_HANDLE m_X3DInstance;

		X3DAUDIO_LISTENER m_Listener = {};
		X3DAUDIO_CONE listenerCone;
		DirectX::XMFLOAT3 *listenerPos = nullptr;
		DirectX::XMFLOAT3 initListenerPos = { 0.0f, 0.0f, 0.0f };



		std::unordered_map<std::string, Sound> m_ReservedSoundData;
		std::unordered_map<std::string, Sound> m_SoundList;

		float m_masterVol = 1.0f;


		SoundManager() {}
		~SoundManager() {};

		static SoundManager *m_instance;

	private:
		bool SerchSoundData(std::string name, Sound &data);
		bool SerchSoundData(std::string name);
		//データの読み込み
		bool Load(const char *name, const char *filename);
		bool Load(std::string name, std::string filename);
		bool Load(Sound &data);

		void InitListener();

		Sound *GetSoundData(std::string name);

		

	public:
		//=================================================
		bool LoadMP3(std::string name, std::string filename);
		//=================================================

		bool Init(unsigned int inputChannel = 0, float sampleRate = 0);
		//bool LoadWaveFile(const char *filename, WAVEFORMATEX &wf, XAUDIO2_BUFFER &buffer, BYTE *&waveData);
		/*void Play();
		void Stop();
		void Release();*/

		void AddSoundData(const char *name, const char *filename, Sound::Tag tag, float vol = 1.0f);

		bool Load(Sound::Tag tag = Sound::Tag::Any);

		void SetMasterVol(float vol)
		{
			m_masterVol = vol;
			m_MasterVoice->SetVolume(vol);
		}

		int PlayBGM2D(std::string name);
		int PlayBGM2D(const char *name);

		int PlaySE2D(std::string name);
		int PlaySE2D(const char *name);

		int PlayBGM3D(std::string name, DirectX::XMFLOAT3 *);
		int PlayBGM3D(const char *name, DirectX::XMFLOAT3 *);

		int PlaySE3D(std::string name, DirectX::XMFLOAT3 *);
		int PlaySE3D(const char *name, DirectX::XMFLOAT3 *);

		void StopBGM2D(std::string name);
		void StopBGM2D(const char *name);

		void StopSE2D(std::string name);
		void StopSE2D(const char *name);

		void StopBGM3D(std::string name);
		void StopBGM3D(const char *name);

		void StopSE3D(std::string name);
		void StopSE3D(const char *name);

		void AllStopBGM2D();

		void SetListenerPos(DirectX::XMFLOAT3 *pos);
		void SetEmitterPos(std::string name, DirectX::XMFLOAT3 *pos);

		void Update3D(const char *name);


		void Uninit();


		static SoundManager *GetInstance()
		{
			if (m_instance == nullptr)
			{
				m_instance = new SoundManager();
			}
			return m_instance;
		}

	};


}


