#include "soundManager.h"

bool Utility::Sound::LoadWaveFile(const char *filename, WAVEFORMATEX &wf, XAUDIO2_BUFFER &buffer, BYTE *&waveData)
{
	FILE *file;
	fopen_s(&file, filename, "rb");
	if (!file) return false;

	// RIFFヘッダーの読み込み
	DWORD chunkSize;
	fread(&chunkSize, sizeof(DWORD), 1, file);
	fseek(file, 20, SEEK_SET);
	fread(&wf, sizeof(WAVEFORMATEX), 1, file);

	// データチャンクの読み込み
	fseek(file, 36, SEEK_SET);
	fread(&chunkSize, sizeof(DWORD), 1, file);
	waveData = new BYTE[chunkSize];
	fread(waveData, chunkSize, 1, file);

	buffer.AudioBytes = chunkSize;
	buffer.pAudioData = waveData;
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	fclose(file);
	return true;
}

bool Utility::Sound::LoadMP3File(const wchar_t *filename, WAVEFORMATEX &wf, XAUDIO2_BUFFER &buffer, BYTE *&waveData)
{
	HRESULT hr;
	IMFSourceReader *pReader = nullptr;
	IMFMediaType *pAudioType = nullptr;
	WAVEFORMATEX *_wf = nullptr;

	hr = MFCreateSourceReaderFromURL(filename, nullptr, &pReader);
	if (FAILED(hr)) return hr;

	hr = MFCreateMediaType(&pAudioType);
	if (FAILED(hr)) return hr;

	pAudioType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	pAudioType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);

	hr = pReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, NULL, pAudioType);
	if (FAILED(hr)) return hr;

	pAudioType->Release();

	// PCMフォーマット情報の取得
	IMFMediaType *pOutputType = nullptr;
	hr = pReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pOutputType);
	if (FAILED(hr)) return hr;

	UINT32 size = 0;
	_wf = (WAVEFORMATEX *)CoTaskMemAlloc(sizeof(WAVEFORMATEX));
	hr = MFCreateWaveFormatExFromMFMediaType(pOutputType, &_wf, &size);
	if (FAILED(hr)) return hr;

	pOutputType->Release();

	// データ読み込み
	DWORD totalBufferSize = 0;
	BYTE *pAudioData = nullptr;

	while (true) {
		DWORD streamIndex, flags;
		IMFSample *pSample = nullptr;

		hr = pReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &streamIndex, &flags, NULL, &pSample);
		if (FAILED(hr) || (flags & MF_SOURCE_READERF_ENDOFSTREAM)) {
			break;
		}

		if (pSample) {
			IMFMediaBuffer *pBuffer = nullptr;
			hr = pSample->ConvertToContiguousBuffer(&pBuffer);
			if (FAILED(hr)) return hr;

			BYTE *pData = nullptr;
			DWORD bufferSize = 0;
			hr = pBuffer->Lock(&pData, nullptr, &bufferSize);
			if (FAILED(hr)) return hr;

			BYTE *newBuffer = new BYTE[totalBufferSize + bufferSize];
			memcpy(newBuffer, pAudioData, totalBufferSize);
			memcpy(newBuffer + totalBufferSize, pData, bufferSize);
			delete[] pAudioData;
			pAudioData = newBuffer;
			totalBufferSize += bufferSize;

			pBuffer->Unlock();
			pBuffer->Release();
			pSample->Release();
		}
	}

	pReader->Release();

	wf = *_wf;
	buffer.AudioBytes = totalBufferSize;
	buffer.pAudioData = pAudioData;
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	return true;
}
	unsigned const short System::Config::Sound::BgmMax_2D = 3;
	unsigned const short System::Config::Sound::SeMax_2D = 30;
	unsigned const short System::Config::Sound::BgmMax_3D = 3;
	unsigned const short System::Config::Sound::SeMax_3D = 30;

namespace ym {
	int SearchSourceVoice(IXAudio2SourceVoice **SourceVoice, int size);
	int SearchSourceVoice(SourceData3D *Sourcedata, int size);



	


	SoundManager *SoundManager::m_instance = nullptr;


	bool SoundManager::Init(unsigned int inputChannel, float sampleRate)
	{
		HRESULT hr;


		

		// Media Foundationの初期化
		hr = MFStartup(MF_VERSION);

		// XAudio2初期化
		hr = XAudio2Create(&m_XAudio2);
		if (FAILED(hr)) return -1;

		hr = m_XAudio2->CreateMasteringVoice(&m_MasterVoice, inputChannel, sampleRate);

		m_voiceDetails = {};
		m_MasterVoice->GetVoiceDetails(&m_voiceDetails);

		//ソースボイスの作成
		m_SrcBgm2D.resize(System::Config::Sound::BgmMax_2D);
		m_SrcSe2D.resize(System::Config::Sound::SeMax_2D);
		m_SrcBgm3D.resize(System::Config::Sound::BgmMax_3D);
		m_SrcSe3D.resize(System::Config::Sound::SeMax_3D);

		// 適当な値で初期化
		WAVEFORMATEX waveFormat = {};
		waveFormat.wFormatTag = WAVE_FORMAT_PCM;
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = 44100;
		waveFormat.wBitsPerSample = 16;
		waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
		waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
		waveFormat.cbSize = 0;

		for (int i = 0; i < m_SrcBgm2D.size(); i++)
		{
			hr = m_XAudio2->CreateSourceVoice(&m_SrcBgm2D[i], &waveFormat);
			if (FAILED(hr)) {
				printf("CreateSourceVoice failed with error: %08X\n", hr);
				return -1;
			}

		}

		for (int i = 0; i < m_SrcSe2D.size(); i++)
		{
			hr = m_XAudio2->CreateSourceVoice(&m_SrcSe2D[i], &waveFormat);
			if (FAILED(hr)) {
				printf("CreateSourceVoice failed with error: %08X\n", hr);
				return -1;
			}

		}
		static X3DAUDIO_DISTANCE_CURVE_POINT volumeCurvePoints[] = {
		{ 0.0f, 1.0f }, // 距離 0.0 のとき、音量は 1.0（最大音量）
		{ 50.0f, 0.5f }, // 距離 5.0 のとき、音量は 0.8
		{ 150.0f, 0.2f }, // 距離 15.0 のとき、音量は 0.2
		{ 200.0f, 0.0f }  // 距離 20.0 のとき、音量は 0.0（聞こえない）
		};

		static X3DAUDIO_DISTANCE_CURVE volumeCurve = {
			volumeCurvePoints,
			_countof(volumeCurvePoints)
		};

		for (int i = 0; i < m_SrcBgm3D.size(); i++)
		{
			hr = m_XAudio2->CreateSourceVoice(&m_SrcBgm3D[i].m_SrcVoice, &waveFormat);
			if (FAILED(hr)) {
				printf("CreateSourceVoice failed with error: %08X\n", hr);
				return -1;
			}
			m_SrcBgm3D[i].m_Emitter.pChannelAzimuths = new float[2] { -X3DAUDIO_PI / 2, X3DAUDIO_PI / 2 };
			m_SrcBgm3D[i].m_Emitter.ChannelRadius = 1.0f; // チャンネル半径
			m_SrcBgm3D[i].m_Emitter.pVolumeCurve = &volumeCurve; // 減衰（距離による音量の変化）の設定


		}


		for (int i = 0; i < m_SrcSe3D.size(); i++)
		{
			hr = m_XAudio2->CreateSourceVoice(&m_SrcSe3D[i].m_SrcVoice, &waveFormat);
			if (FAILED(hr)) {
				printf("CreateSourceVoice failed with error: %08X\n", hr);
				return -1;
			}
			m_SrcSe3D[i].m_Emitter.pChannelAzimuths = new float[2] { -X3DAUDIO_PI / 2, X3DAUDIO_PI / 2 };

			/*float *matrix = new float[2*8] {0.0f};
			m_SrcSe3D[i].m_DSPSettings.pMatrixCoefficients =matrix;*/
			m_SrcSe3D[i].m_DSPSettings.pDelayTimes = new float[m_voiceDetails.InputChannels];
		}


		// X3DAudio初期化

		m_MasterVoice->GetChannelMask(&m_dwChannelMask);


		hr = X3DAudioInitialize(m_dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, m_X3DInstance);
		if (FAILED(hr)) {
			printf("X3DAudioInitialize failed with error: %08X\n", hr);
			return -1;
		}

		InitListener();



		return true;
	}

	void SoundManager::Uninit()
	{

		for (auto &src : m_SrcBgm2D)
		{
			if (src)
			{
				src->DestroyVoice();
			}
		}
		for (auto &src : m_SrcSe2D)
		{
			if (src)
			{
				src->DestroyVoice();
			}
		}
		for (auto &src : m_SrcBgm3D)
		{
			if (src.m_SrcVoice)
			{
				src.m_SrcVoice->DestroyVoice();
			}
		}
		for (auto &src : m_SrcSe3D)
		{
			if (src.m_SrcVoice)
			{
				src.m_SrcVoice->DestroyVoice();
			}
		}

		if (m_MasterVoice)
			m_MasterVoice->DestroyVoice();
		if (m_XAudio2)
			m_XAudio2->Release();

		m_SoundList.clear();
		m_ReservedSoundData.clear();


		listenerPos = nullptr;

		CoUninitialize();
	}

	bool SoundManager::Load(Sound::Tag tag)
	{
		for (auto &data : m_ReservedSoundData)
		{
			if (data.second.GetTag() == tag)
			{
				Load(data.second);

			}
		}
		//読み込みが完了したタグのデータをクリア

		for (auto it = m_ReservedSoundData.begin(); it != m_ReservedSoundData.end();)
		{
			if (it->second.GetTag() == tag)
			{
				it = m_ReservedSoundData.erase(it);
			}
			else
			{
				++it;
			}
		}





		return true;
	}

	void SoundManager::InitListener()
	{
		m_Listener = {};
		listenerPos = &initListenerPos;
		m_Listener.Position = { listenerPos->x, listenerPos->y, listenerPos->z };
		m_Listener.OrientFront = { 0.0f, 0.0f, 1.0f };
		m_Listener.OrientTop = { 0.0f, 1.0f, 0.0f };
		m_Listener.Velocity = { 0.0f, 0.0f, 0.0f };
		listenerCone = {
		X3DAUDIO_PI / 4,    // InnerAngle: 前方45度は減衰なし
		X3DAUDIO_PI,        // OuterAngle: 180度まで外側として減衰開始
		1.0f,               // InnerVolume: 内側は音量そのまま
		0.5f,               // OuterVolume: 背面方向は音量半分
		1.0f,               // InnerLPF: フィルタなし
		0.8f,               // OuterLPF: 少しこもらせる
		1.0f,               // InnerReverb: 内側のリバーブ
		0.2f                // OuterReverb: 背面はリバーブを少なく
		};
		m_Listener.pCone = &listenerCone;
	}

	Sound *SoundManager::GetSoundData(std::string name)
	{
		if (m_SoundList.find(name) == m_SoundList.end())
		{
			return nullptr;
		}
		return &m_SoundList[name];
		
	}

	

	void SoundManager::AddSoundData(const char *name, const char *filename, Sound::Tag tag, float vol)
	{
		if (m_ReservedSoundData.find(name) != m_ReservedSoundData.end())
		{
			return;
		}
		Sound sd(name, filename, tag, vol);
		m_ReservedSoundData[name] = sd;

	}

	bool SoundManager::Load(const char *name, const char *filename)
	{
		if (SerchSoundData(name))
		{
			return true;
		}
		printf("load\n");
		Sound sd;
		sd.Load(name, filename);
		m_SoundList[name] = sd;
		return true;
	}

	bool SoundManager::Load(std::string name, std::string filename)
	{
		return Load(name.c_str(), filename.c_str());
	}
	bool SoundManager::LoadMP3(std::string name, std::string filename)
	{
		Sound sd;
		sd.LoadMP3(name, filename);
		m_SoundList[name] = sd;
		return true;
	}

	bool SoundManager::Load(Sound &data)
	{
		if (SerchSoundData(data.GetName()))
		{
			return true;
		}
		printf("load\n");
		Sound sd = data;
		sd.Load(data.GetName(), data.GetFileName());
		m_SoundList[data.GetName()] = sd;
		return true;
	}

	int SoundManager::PlayBGM2D(std::string name)
	{
		return PlayBGM2D(name.c_str());
	}
	int SoundManager::PlayBGM2D(const char *name)
	{
		Sound data;
		if (!SerchSoundData(name, data))
		{
			printf("not found[%s]\n", name);
			return -1;
		}
		// 使用できる再生リソースを検索
		int sourceVoiceNum = SearchSourceVoice(m_SrcBgm2D.data(), m_SrcBgm2D.size());
		if (sourceVoiceNum == -1)
		{
			return -1;
		}
		//音量の設定
		m_SrcBgm2D[sourceVoiceNum]->SetVolume(data.GetVol());

		// ソースボイスの再生
		m_SrcBgm2D[sourceVoiceNum]->SubmitSourceBuffer(&data.GetBuffer());

		m_SrcBgm2D[sourceVoiceNum]->Start(0);
		return 0;
	}
	int SoundManager::PlaySE2D(std::string name)
	{
		return PlaySE2D(name.c_str());
	}

	int SoundManager::PlaySE2D(const char *name)
	{
		Sound data;
		if (!SerchSoundData(name, data))
		{
			ym::ConsoleLogRelease("not found[%s]\n", name);
			return -1;
		}
		// 使用できる再生リソースを検索
		int sourceVoiceNum = SearchSourceVoice(m_SrcSe2D.data(), m_SrcSe2D.size());
		if (sourceVoiceNum == -1)
		{
			ym::ConsoleLogRelease("リソースがない\n");
			return -1;
		}
		//音量の設定
		m_SrcSe2D[sourceVoiceNum]->SetVolume(data.GetVol());

		// ソースボイスの再生
		m_SrcSe2D[sourceVoiceNum]->SubmitSourceBuffer(&data.GetBuffer());

		m_SrcSe2D[sourceVoiceNum]->Start(0);
		return 0;
	}


	void SoundManager::Update3D(const char *name)
	{

		Sound data;
		if (!SerchSoundData(name, data))
		{
			printf("not found[%s]\n", name);
			return;
		}

		int sourceVoiceNum = -1;
		for (int i = 0; i < m_SrcBgm3D.size(); i++)
		{
			// 現在の状態を取得
			XAUDIO2_VOICE_STATE state;
			m_SrcBgm3D[i].m_SrcVoice->GetState(&state);
			// バッファが0ならば再生可能と判断
			if (state.BuffersQueued != 0)
			{
				sourceVoiceNum = i;
				DirectX::XMFLOAT3 _pos = *m_SrcBgm3D[sourceVoiceNum].m_pos;
				m_SrcBgm3D[sourceVoiceNum].m_Emitter.Position = { _pos.x, _pos.y,_pos.z };

				//座標の表示
				printf("エミッターpos[%f,%f,%f]\n", _pos.x, _pos.y, _pos.z);
				//リスナー
				m_Listener.Position = { listenerPos->x, listenerPos->y, listenerPos->z };
				printf("リスナーpos[%f,%f,%f]\n", listenerPos->x, listenerPos->y, listenerPos->z);

				//音量の設定
				m_SrcBgm3D[sourceVoiceNum].m_SrcVoice->SetVolume(data.GetVol());

				int flag = X3DAUDIO_CALCULATE_MATRIX;


				// 3Dオーディオ計算
				X3DAudioCalculate(m_X3DInstance, &m_Listener, &m_SrcBgm3D[sourceVoiceNum].m_Emitter, flag, &m_SrcBgm3D[sourceVoiceNum].m_DSPSettings);

				auto dspSettings = m_SrcBgm3D[sourceVoiceNum].m_DSPSettings;
				for (int i = 0; i < dspSettings.SrcChannelCount * dspSettings.DstChannelCount; ++i) {

					printf("Matrix coefficient %d: %f\n", i, dspSettings.pMatrixCoefficients[i]);

				}

				// ソースボイスの再生
			//	m_SrcBgm3D[sourceVoiceNum].m_SrcVoice->SubmitSourceBuffer(&data.GetBuffer());

				m_SrcBgm3D[sourceVoiceNum].m_SrcVoice->SetFrequencyRatio(m_SrcBgm3D[sourceVoiceNum].m_DSPSettings.DopplerFactor);

				//行列

				m_SrcBgm3D[sourceVoiceNum].m_SrcVoice->SetOutputMatrix(m_MasterVoice, m_SrcBgm3D[sourceVoiceNum].m_DSPSettings.SrcChannelCount, m_SrcBgm3D[sourceVoiceNum].m_DSPSettings.DstChannelCount, m_SrcBgm3D[sourceVoiceNum].m_DSPSettings.pMatrixCoefficients);

				//break;
			}
		}

		for (int i = 0; i < m_SrcSe3D.size(); i++)
		{
			// 現在の状態を取得
			XAUDIO2_VOICE_STATE state;
			m_SrcSe3D[i].m_SrcVoice->GetState(&state);
			// バッファが0ならば再生可能と判断
			if (state.BuffersQueued != 0)
			{
				sourceVoiceNum = i;
				DirectX::XMFLOAT3 _pos = *m_SrcSe3D[sourceVoiceNum].m_pos;
				m_SrcSe3D[sourceVoiceNum].m_Emitter.Position = { _pos.x, _pos.y,_pos.z };

				//座標の表示
				printf("エミッターpos[%f,%f,%f]\n", _pos.x, _pos.y, _pos.z);
				m_Listener.Position = { listenerPos->x, listenerPos->y, listenerPos->z };
				//リスナー
				printf("リスナーpos[%f,%f,%f]\n", listenerPos->x, listenerPos->y, listenerPos->z);

				//音量の設定
				m_SrcSe3D[sourceVoiceNum].m_SrcVoice->SetVolume(data.GetVol());

				int flag = X3DAUDIO_CALCULATE_MATRIX;


				// 3Dオーディオ計算
				X3DAudioCalculate(m_X3DInstance, &m_Listener, &m_SrcSe3D[sourceVoiceNum].m_Emitter, flag, &m_SrcSe3D[sourceVoiceNum].m_DSPSettings);

				auto dspSettings = m_SrcBgm3D[sourceVoiceNum].m_DSPSettings;
				for (int i = 0; i < dspSettings.SrcChannelCount * dspSettings.DstChannelCount; ++i) {

					printf("Matrix coefficient %d: %f\n", i, dspSettings.pMatrixCoefficients[i]);

				}

				// ソースボイスの再生
				//	m_SrcBgm3D[sourceVoiceNum].m_SrcVoice->SubmitSourceBuffer(&data.GetBuffer());

				m_SrcSe3D[sourceVoiceNum].m_SrcVoice->SetFrequencyRatio(m_SrcSe3D[sourceVoiceNum].m_DSPSettings.DopplerFactor);

				//行列

				m_SrcSe3D[sourceVoiceNum].m_SrcVoice->SetOutputMatrix(m_MasterVoice, m_SrcSe3D[sourceVoiceNum].m_DSPSettings.SrcChannelCount, m_SrcSe3D[sourceVoiceNum].m_DSPSettings.DstChannelCount, m_SrcSe3D[sourceVoiceNum].m_DSPSettings.pMatrixCoefficients);
			}
		}



	}

	int SoundManager::PlaySE3D(std::string name, DirectX::XMFLOAT3 *pos)
	{
		return PlaySE3D(name.c_str(), pos);
	}

	int SoundManager::PlaySE3D(const char *name, DirectX::XMFLOAT3 *pos)
	{

		Sound data;
		if (!SerchSoundData(name, data))
		{
			printf("not found[%s]\n", name);
			return -1;
		}
		// 使用できる再生リソースを検索
		int sourceVoiceNum = SearchSourceVoice(m_SrcSe3D.data(), m_SrcSe3D.size());
		if (sourceVoiceNum == -1)
		{
			return -1;
		}
		printf("se[%d]", sourceVoiceNum);
		//エミッターの設定
		m_SrcSe3D[sourceVoiceNum].m_pos = pos;
		DirectX::XMFLOAT3 _pos = *m_SrcSe3D[sourceVoiceNum].m_pos;
		m_SrcSe3D[sourceVoiceNum].m_Emitter.Position = { _pos.x, _pos.y,_pos.z };
		m_SrcSe3D[sourceVoiceNum].m_Emitter.OrientFront = { 0.0f, 0.0f, 1.0f };
		m_SrcSe3D[sourceVoiceNum].m_Emitter.OrientTop = { 0.0f, 1.0f, 0.0f };
		m_SrcSe3D[sourceVoiceNum].m_Emitter.Velocity = { 0.0f, 0.0f, 0.0f };
		m_SrcSe3D[sourceVoiceNum].m_Emitter.pChannelAzimuths = new float[2] { -X3DAUDIO_PI / 2, X3DAUDIO_PI / 2 };	// エミッターの方位角
		m_SrcSe3D[sourceVoiceNum].m_Emitter.ChannelCount = data.GetChannelCount(); // チャンネル数
		m_SrcSe3D[sourceVoiceNum].m_Emitter.ChannelRadius = 1.0f; // チャンネル半径
		m_SrcSe3D[sourceVoiceNum].m_Emitter.CurveDistanceScaler = m_SrcSe3D[sourceVoiceNum].m_Emitter.DopplerScaler = 1.0f; // 距離スケーラー
		//m_SrcSe3D[sourceVoiceNum].m_Emitter.pVolumeCurve = data.GetVolumeCurve(); // 減衰（距離による音量の

		//DSP設定
		m_SrcSe3D[sourceVoiceNum].m_DSPSettings = { 0 };
		m_SrcSe3D[sourceVoiceNum].m_DSPSettings.SrcChannelCount = m_SrcSe3D[sourceVoiceNum].m_Emitter.ChannelCount; // エミッターのチャンネル数
		m_SrcSe3D[sourceVoiceNum].m_DSPSettings.DstChannelCount = m_voiceDetails.InputChannels;
		float *matrix = new float[m_SrcSe3D[sourceVoiceNum].m_DSPSettings.SrcChannelCount * m_SrcSe3D[sourceVoiceNum].m_DSPSettings.DstChannelCount] {0.0f};
		m_SrcSe3D[sourceVoiceNum].m_DSPSettings.pMatrixCoefficients = matrix;
		m_SrcSe3D[sourceVoiceNum].m_DSPSettings.pDelayTimes = new float[m_SrcSe3D[sourceVoiceNum].m_DSPSettings.DstChannelCount];
		if (!m_SrcSe3D[sourceVoiceNum].m_DSPSettings.pMatrixCoefficients) {
			printf("Failed to allocate memory for pMatrixCoefficients\n");
			//delete[] data.GetWaveData();
			return -1;
		}
		int flag = X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER | X3DAUDIO_CALCULATE_DELAY;

		//音量の設定
		m_SrcSe3D[sourceVoiceNum].m_SrcVoice->SetVolume(data.GetVol());

		// 3Dオーディオ計算
		X3DAudioCalculate(m_X3DInstance, &m_Listener, &m_SrcSe3D[sourceVoiceNum].m_Emitter, flag, &m_SrcSe3D[sourceVoiceNum].m_DSPSettings);

		// ソースボイスの再生

		m_SrcSe3D[sourceVoiceNum].m_SrcVoice->SetFrequencyRatio(m_SrcSe3D[sourceVoiceNum].m_DSPSettings.DopplerFactor);

		//行列

		m_SrcSe3D[sourceVoiceNum].m_SrcVoice->SetOutputMatrix(m_MasterVoice, m_SrcSe3D[sourceVoiceNum].m_DSPSettings.SrcChannelCount, m_SrcSe3D[sourceVoiceNum].m_DSPSettings.DstChannelCount, m_SrcSe3D[sourceVoiceNum].m_DSPSettings.pMatrixCoefficients);
		m_SrcSe3D[sourceVoiceNum].m_SrcVoice->SubmitSourceBuffer(&data.GetBuffer());

		//再生
		m_SrcSe3D[sourceVoiceNum].m_SrcVoice->Start(0);
	}

	void SoundManager::StopBGM2D(std::string name)
	{

		StopBGM2D(name.c_str());
	}

	void SoundManager::StopBGM2D(const char *name)
	{
		
		

		//m_SrcBgm2D[sourceVoiceNum]->Stop();


	}

	void SoundManager::StopSE2D(std::string name)
	{
		Sound data;
		if (!SerchSoundData(name, data))
		{
			printf("not found[%s]\n", name);
			return;
		}
		// 使用できる再生リソースを検索
		int sourceVoiceNum = SearchSourceVoice(m_SrcSe2D.data(), m_SrcSe2D.size());
		if (sourceVoiceNum == -1)
		{
			return;
		}
		//停止
		m_SrcSe2D[sourceVoiceNum]->Stop();
	}

	void SoundManager::StopSE2D(const char *name)
	{
		Sound data;
		if (!SerchSoundData(name, data))
		{
			printf("not found[%s]\n", name);
			return;
		}
		// 使用できる再生リソースを検索
		int sourceVoiceNum = SearchSourceVoice(m_SrcSe2D.data(), m_SrcSe2D.size());
		if (sourceVoiceNum == -1)
		{
			return;
		}
		//停止
		m_SrcSe2D[sourceVoiceNum]->Stop();
	}

	void SoundManager::StopBGM3D(std::string name)
	{
		Sound data;
		if (!SerchSoundData(name, data))
		{
			printf("not found[%s]\n", name);
			return;
		}
		// 使用できる再生リソースを検索
		int sourceVoiceNum = SearchSourceVoice(m_SrcBgm3D.data(), m_SrcBgm3D.size());
		if (sourceVoiceNum == -1)
		{
			return;
		}
		//停止
		m_SrcBgm3D[sourceVoiceNum].m_SrcVoice->Stop();

	}

	void SoundManager::StopBGM3D(const char *name)
	{
		Sound data;
		if (!SerchSoundData(name, data))
		{
			printf("not found[%s]\n", name);
			return;
		}
		// 使用できる再生リソースを検索
		int sourceVoiceNum = SearchSourceVoice(m_SrcBgm3D.data(), m_SrcBgm3D.size());
		if (sourceVoiceNum == -1)
		{
			return;
		}
		//停止
		m_SrcBgm3D[sourceVoiceNum].m_SrcVoice->Stop();
	}

	void SoundManager::StopSE3D(std::string name)
	{
		Sound data;
		if (!SerchSoundData(name, data))
		{
			printf("not found[%s]\n", name);
			return;
		}
		// 使用できる再生リソースを検索
		int sourceVoiceNum = SearchSourceVoice(m_SrcSe3D.data(), m_SrcSe3D.size());
		if (sourceVoiceNum == -1)
		{
			return;
		}
		//停止
		m_SrcSe3D[sourceVoiceNum].m_SrcVoice->Stop();
	}

	void SoundManager::StopSE3D(const char *name)
	{
		Sound data;
		if (!SerchSoundData(name, data))
		{
			printf("not found[%s]\n", name);
			return;
		}
		// 使用できる再生リソースを検索
		int sourceVoiceNum = SearchSourceVoice(m_SrcSe3D.data(), m_SrcSe3D.size());
		if (sourceVoiceNum == -1)
		{
			return;
		}
		//停止
		m_SrcSe3D[sourceVoiceNum].m_SrcVoice->Stop();
	}

	void SoundManager::AllStopBGM2D()
	{
		for (auto &src : m_SrcBgm2D)
		{
			src->Stop();
			src->FlushSourceBuffers();
		}
	}

	void SoundManager::SetListenerPos(DirectX::XMFLOAT3 *pos)
	{
		listenerPos = pos;
		m_Listener.Position = { pos->x, pos->y, pos->z };
	}

	void SoundManager::SetEmitterPos(std::string name, DirectX::XMFLOAT3 *pos)
	{
		Sound data;
		if (!SerchSoundData(name, data))
		{
			printf("not found[%s]\n", name);
			return;
		}
		// 使用できる再生リソースを検索
		int sourceVoiceNum = SearchSourceVoice(m_SrcBgm3D.data(), m_SrcBgm3D.size());
		if (sourceVoiceNum == -1)
		{
			return;
		}
		//エミッターの設定
		m_SrcBgm3D[sourceVoiceNum].m_pos = pos;
		DirectX::XMFLOAT3 _pos = *m_SrcBgm3D[sourceVoiceNum].m_pos;
		m_SrcBgm3D[sourceVoiceNum].m_Emitter.Position = { _pos.x, _pos.y,_pos.z };
	}

	int SoundManager::PlayBGM3D(std::string name, DirectX::XMFLOAT3 *pos)
	{
		return PlayBGM3D(name.c_str(), pos);
	}

	int SoundManager::PlayBGM3D(const char *name, DirectX::XMFLOAT3 *pos)
	{
		Sound data;
		if (!SerchSoundData(name, data))
		{
			printf("not found[%s]\n", name);
			return -1;
		}
		// 使用できる再生リソースを検索
		int sourceVoiceNum = SearchSourceVoice(m_SrcBgm3D.data(), m_SrcBgm3D.size());
		if (sourceVoiceNum == -1)
		{
			return -1;
		}
		//エミッターの設定
		m_SrcBgm3D[sourceVoiceNum].m_pos = pos;
		DirectX::XMFLOAT3 _pos = *m_SrcBgm3D[sourceVoiceNum].m_pos;
		m_SrcBgm3D[sourceVoiceNum].m_Emitter.Position = { _pos.x, _pos.y,_pos.z };
		m_SrcBgm3D[sourceVoiceNum].m_Emitter.OrientFront = { 0.0f, 0.0f, 1.0f };
		m_SrcBgm3D[sourceVoiceNum].m_Emitter.OrientTop = { 0.0f, 1.0f, 0.0f };
		m_SrcBgm3D[sourceVoiceNum].m_Emitter.Velocity = { 0.0f, 0.0f, 0.0f };
		m_SrcBgm3D[sourceVoiceNum].m_Emitter.pChannelAzimuths = new float[2] { -X3DAUDIO_PI / 2, X3DAUDIO_PI / 2 };	// エミッターの方位角
		m_SrcBgm3D[sourceVoiceNum].m_Emitter.ChannelCount = data.GetChannelCount(); // チャンネル数
		m_SrcBgm3D[sourceVoiceNum].m_Emitter.ChannelRadius = 1.0f; // チャンネル半径
		m_SrcBgm3D[sourceVoiceNum].m_Emitter.CurveDistanceScaler = m_SrcBgm3D[sourceVoiceNum].m_Emitter.DopplerScaler = 1.0f; // 距離スケーラー
		//m_SrcBgm3D[sourceVoiceNum].m_Emitter.pVolumeCurve = data.GetVolumeCurve(); // 減衰（距離による音量の変化）の設定

		//DSP設定
		m_SrcBgm3D[sourceVoiceNum].m_DSPSettings = { 0 };
		m_SrcBgm3D[sourceVoiceNum].m_DSPSettings.SrcChannelCount = m_SrcBgm3D[sourceVoiceNum].m_Emitter.ChannelCount; // エミッターのチャンネル数
		m_SrcBgm3D[sourceVoiceNum].m_DSPSettings.DstChannelCount = m_voiceDetails.InputChannels;
		float *matrix = new float[m_SrcBgm3D[sourceVoiceNum].m_DSPSettings.SrcChannelCount * m_SrcBgm3D[sourceVoiceNum].m_DSPSettings.DstChannelCount] {0.0f};
		m_SrcBgm3D[sourceVoiceNum].m_DSPSettings.pMatrixCoefficients = matrix;
		m_SrcBgm3D[sourceVoiceNum].m_DSPSettings.pDelayTimes = new float[m_SrcBgm3D[sourceVoiceNum].m_DSPSettings.DstChannelCount];
		if (!m_SrcBgm3D[sourceVoiceNum].m_DSPSettings.pMatrixCoefficients) {
			printf("Failed to allocate memory for pMatrixCoefficients\n");
			//delete[] data.GetWaveData();
			return -1;
		}
		int flag = X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER | X3DAUDIO_CALCULATE_DELAY;

		//音量の設定
		m_SrcBgm3D[sourceVoiceNum].m_SrcVoice->SetVolume(data.GetVol());

		// 3Dオーディオ計算
		X3DAudioCalculate(m_X3DInstance, &m_Listener, &m_SrcBgm3D[sourceVoiceNum].m_Emitter, flag, &m_SrcBgm3D[sourceVoiceNum].m_DSPSettings);

		// ソースボイスの再生

		m_SrcBgm3D[sourceVoiceNum].m_SrcVoice->SetFrequencyRatio(m_SrcBgm3D[sourceVoiceNum].m_DSPSettings.DopplerFactor);

		//行列

		m_SrcBgm3D[sourceVoiceNum].m_SrcVoice->SetOutputMatrix(m_MasterVoice, m_SrcBgm3D[sourceVoiceNum].m_DSPSettings.SrcChannelCount, m_SrcBgm3D[sourceVoiceNum].m_DSPSettings.DstChannelCount, m_SrcBgm3D[sourceVoiceNum].m_DSPSettings.pMatrixCoefficients);
		m_SrcBgm3D[sourceVoiceNum].m_SrcVoice->SubmitSourceBuffer(&data.GetBuffer());

		//再生
		m_SrcBgm3D[sourceVoiceNum].m_SrcVoice->Start(0);
		return 0;

	}

	bool SoundManager::SerchSoundData(std::string name)
	{
		if (m_SoundList.find(name) == m_SoundList.end())
		{
			return false;
		}
		return true;
	}
	bool SoundManager::SerchSoundData(std::string name, Sound &data)
	{
		if (m_SoundList.find(name) == m_SoundList.end())
		{
			return false;
		}
		data = m_SoundList[name];
		return true;
	}

	//===================================================
	// 複数のソースボイスから使用できるリソースを検索
	//===================================================
	int SearchSourceVoice(IXAudio2SourceVoice **SourceVoice, int size)
	{
		// 今回再生するリソース
		int sourceVoiceNum = -1;

		// リソースのバッファ
		unsigned int soundBuffer = 0;

		// 使用できる再生リソースを検索
		for (int i = 0; i < size; i++)
		{
			// 現在の状態を取得
			XAUDIO2_VOICE_STATE state;
			SourceVoice[i]->GetState(&state);

			// バッファが0ならば再生可能と判断
			if (state.BuffersQueued == 0)
			{
				sourceVoiceNum = i;
				break;
			}
			else
			{
				// 初期値もしくはバッファが最小の場合は入れ替え
				if (soundBuffer == 0 || soundBuffer > state.BuffersQueued)
				{
					soundBuffer = state.BuffersQueued;
					sourceVoiceNum = i;
				}
			}
		}

		return sourceVoiceNum;
	}

	int SearchSourceVoice(SourceData3D *Sourcedata, int size)
	{
		// 今回再生するリソース
		int sourceVoiceNum = -1;

		// リソースのバッファ
		unsigned int soundBuffer = 0;

		// 使用できる再生リソースを検索
		for (int i = 0; i < size; i++)
		{
			// 現在の状態を取得
			XAUDIO2_VOICE_STATE state;
			Sourcedata[i].m_SrcVoice->GetState(&state);

			// バッファが0ならば再生可能と判断
			if (state.BuffersQueued == 0)
			{
				sourceVoiceNum = i;
				break;
			}
			else
			{
				// 初期値もしくはバッファが最小の場合は入れ替え
				if (soundBuffer == 0 || soundBuffer > state.BuffersQueued)
				{
					soundBuffer = state.BuffersQueued;
					sourceVoiceNum = i;
				}
			}
		}

		return sourceVoiceNum;
	}


	bool Sound::Load(const char *name, const char *filename)
	{

		if (!Utility::Sound::LoadWaveFile(filename, wf, buffer, waveData))
		{
			return false;
		}
		m_name = name;
		m_fileName = filename;
		isLoaded = true;



		return true;



	}
	bool Sound::Load(std::string name, std::string filename)
	{
		if (!Utility::Sound::LoadWaveFile(filename.c_str(), wf, buffer, waveData))
		{
			return false;
		}
		m_name = name;
		m_fileName = filename;
		isLoaded = true;
		buffer.LoopBegin = buffer.PlayBegin = 0;
		return true;
	}

	bool Sound::LoadMP3(std::string name, std::string filename)
	{

		auto wfilename = ym::Utf8ToUtf16(filename);

		if (!Utility::Sound::LoadMP3File(wfilename.c_str(), wf, buffer, waveData))
		{
			return false;
		}
		m_name = name;
		m_fileName = filename;
		isLoaded = true;
	}

}
