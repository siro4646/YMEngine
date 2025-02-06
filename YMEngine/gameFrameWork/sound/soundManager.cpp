#include "soundManager.h"

bool Utility::Sound::LoadWaveFile(const char *filename, WAVEFORMATEX &wf, XAUDIO2_BUFFER &buffer, BYTE *&waveData)
{
	FILE *file;
	fopen_s(&file, filename, "rb");
	if (!file) return false;

	// RIFF�w�b�_�[�̓ǂݍ���
	DWORD chunkSize;
	fread(&chunkSize, sizeof(DWORD), 1, file);
	fseek(file, 20, SEEK_SET);
	fread(&wf, sizeof(WAVEFORMATEX), 1, file);

	// �f�[�^�`�����N�̓ǂݍ���
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

	// PCM�t�H�[�}�b�g���̎擾
	IMFMediaType *pOutputType = nullptr;
	hr = pReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pOutputType);
	if (FAILED(hr)) return hr;

	UINT32 size = 0;
	_wf = (WAVEFORMATEX *)CoTaskMemAlloc(sizeof(WAVEFORMATEX));
	hr = MFCreateWaveFormatExFromMFMediaType(pOutputType, &_wf, &size);
	if (FAILED(hr)) return hr;

	pOutputType->Release();

	// �f�[�^�ǂݍ���
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


		

		// Media Foundation�̏�����
		hr = MFStartup(MF_VERSION);

		// XAudio2������
		hr = XAudio2Create(&m_XAudio2);
		if (FAILED(hr)) return -1;

		hr = m_XAudio2->CreateMasteringVoice(&m_MasterVoice, inputChannel, sampleRate);

		m_voiceDetails = {};
		m_MasterVoice->GetVoiceDetails(&m_voiceDetails);

		//�\�[�X�{�C�X�̍쐬
		m_SrcBgm2D.resize(System::Config::Sound::BgmMax_2D);
		m_SrcSe2D.resize(System::Config::Sound::SeMax_2D);
		m_SrcBgm3D.resize(System::Config::Sound::BgmMax_3D);
		m_SrcSe3D.resize(System::Config::Sound::SeMax_3D);

		// �K���Ȓl�ŏ�����
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
		{ 0.0f, 1.0f }, // ���� 0.0 �̂Ƃ��A���ʂ� 1.0�i�ő剹�ʁj
		{ 50.0f, 0.5f }, // ���� 5.0 �̂Ƃ��A���ʂ� 0.8
		{ 150.0f, 0.2f }, // ���� 15.0 �̂Ƃ��A���ʂ� 0.2
		{ 200.0f, 0.0f }  // ���� 20.0 �̂Ƃ��A���ʂ� 0.0�i�������Ȃ��j
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
			m_SrcBgm3D[i].m_Emitter.ChannelRadius = 1.0f; // �`�����l�����a
			m_SrcBgm3D[i].m_Emitter.pVolumeCurve = &volumeCurve; // �����i�����ɂ�鉹�ʂ̕ω��j�̐ݒ�


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


		// X3DAudio������

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
		//�ǂݍ��݂����������^�O�̃f�[�^���N���A

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
		X3DAUDIO_PI / 4,    // InnerAngle: �O��45�x�͌����Ȃ�
		X3DAUDIO_PI,        // OuterAngle: 180�x�܂ŊO���Ƃ��Č����J�n
		1.0f,               // InnerVolume: �����͉��ʂ��̂܂�
		0.5f,               // OuterVolume: �w�ʕ����͉��ʔ���
		1.0f,               // InnerLPF: �t�B���^�Ȃ�
		0.8f,               // OuterLPF: ���������点��
		1.0f,               // InnerReverb: �����̃��o�[�u
		0.2f                // OuterReverb: �w�ʂ̓��o�[�u�����Ȃ�
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
		// �g�p�ł���Đ����\�[�X������
		int sourceVoiceNum = SearchSourceVoice(m_SrcBgm2D.data(), m_SrcBgm2D.size());
		if (sourceVoiceNum == -1)
		{
			return -1;
		}
		//���ʂ̐ݒ�
		m_SrcBgm2D[sourceVoiceNum]->SetVolume(data.GetVol());

		// �\�[�X�{�C�X�̍Đ�
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
		// �g�p�ł���Đ����\�[�X������
		int sourceVoiceNum = SearchSourceVoice(m_SrcSe2D.data(), m_SrcSe2D.size());
		if (sourceVoiceNum == -1)
		{
			ym::ConsoleLogRelease("���\�[�X���Ȃ�\n");
			return -1;
		}
		//���ʂ̐ݒ�
		m_SrcSe2D[sourceVoiceNum]->SetVolume(data.GetVol());

		// �\�[�X�{�C�X�̍Đ�
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
			// ���݂̏�Ԃ��擾
			XAUDIO2_VOICE_STATE state;
			m_SrcBgm3D[i].m_SrcVoice->GetState(&state);
			// �o�b�t�@��0�Ȃ�΍Đ��\�Ɣ��f
			if (state.BuffersQueued != 0)
			{
				sourceVoiceNum = i;
				DirectX::XMFLOAT3 _pos = *m_SrcBgm3D[sourceVoiceNum].m_pos;
				m_SrcBgm3D[sourceVoiceNum].m_Emitter.Position = { _pos.x, _pos.y,_pos.z };

				//���W�̕\��
				printf("�G�~�b�^�[pos[%f,%f,%f]\n", _pos.x, _pos.y, _pos.z);
				//���X�i�[
				m_Listener.Position = { listenerPos->x, listenerPos->y, listenerPos->z };
				printf("���X�i�[pos[%f,%f,%f]\n", listenerPos->x, listenerPos->y, listenerPos->z);

				//���ʂ̐ݒ�
				m_SrcBgm3D[sourceVoiceNum].m_SrcVoice->SetVolume(data.GetVol());

				int flag = X3DAUDIO_CALCULATE_MATRIX;


				// 3D�I�[�f�B�I�v�Z
				X3DAudioCalculate(m_X3DInstance, &m_Listener, &m_SrcBgm3D[sourceVoiceNum].m_Emitter, flag, &m_SrcBgm3D[sourceVoiceNum].m_DSPSettings);

				auto dspSettings = m_SrcBgm3D[sourceVoiceNum].m_DSPSettings;
				for (int i = 0; i < dspSettings.SrcChannelCount * dspSettings.DstChannelCount; ++i) {

					printf("Matrix coefficient %d: %f\n", i, dspSettings.pMatrixCoefficients[i]);

				}

				// �\�[�X�{�C�X�̍Đ�
			//	m_SrcBgm3D[sourceVoiceNum].m_SrcVoice->SubmitSourceBuffer(&data.GetBuffer());

				m_SrcBgm3D[sourceVoiceNum].m_SrcVoice->SetFrequencyRatio(m_SrcBgm3D[sourceVoiceNum].m_DSPSettings.DopplerFactor);

				//�s��

				m_SrcBgm3D[sourceVoiceNum].m_SrcVoice->SetOutputMatrix(m_MasterVoice, m_SrcBgm3D[sourceVoiceNum].m_DSPSettings.SrcChannelCount, m_SrcBgm3D[sourceVoiceNum].m_DSPSettings.DstChannelCount, m_SrcBgm3D[sourceVoiceNum].m_DSPSettings.pMatrixCoefficients);

				//break;
			}
		}

		for (int i = 0; i < m_SrcSe3D.size(); i++)
		{
			// ���݂̏�Ԃ��擾
			XAUDIO2_VOICE_STATE state;
			m_SrcSe3D[i].m_SrcVoice->GetState(&state);
			// �o�b�t�@��0�Ȃ�΍Đ��\�Ɣ��f
			if (state.BuffersQueued != 0)
			{
				sourceVoiceNum = i;
				DirectX::XMFLOAT3 _pos = *m_SrcSe3D[sourceVoiceNum].m_pos;
				m_SrcSe3D[sourceVoiceNum].m_Emitter.Position = { _pos.x, _pos.y,_pos.z };

				//���W�̕\��
				printf("�G�~�b�^�[pos[%f,%f,%f]\n", _pos.x, _pos.y, _pos.z);
				m_Listener.Position = { listenerPos->x, listenerPos->y, listenerPos->z };
				//���X�i�[
				printf("���X�i�[pos[%f,%f,%f]\n", listenerPos->x, listenerPos->y, listenerPos->z);

				//���ʂ̐ݒ�
				m_SrcSe3D[sourceVoiceNum].m_SrcVoice->SetVolume(data.GetVol());

				int flag = X3DAUDIO_CALCULATE_MATRIX;


				// 3D�I�[�f�B�I�v�Z
				X3DAudioCalculate(m_X3DInstance, &m_Listener, &m_SrcSe3D[sourceVoiceNum].m_Emitter, flag, &m_SrcSe3D[sourceVoiceNum].m_DSPSettings);

				auto dspSettings = m_SrcBgm3D[sourceVoiceNum].m_DSPSettings;
				for (int i = 0; i < dspSettings.SrcChannelCount * dspSettings.DstChannelCount; ++i) {

					printf("Matrix coefficient %d: %f\n", i, dspSettings.pMatrixCoefficients[i]);

				}

				// �\�[�X�{�C�X�̍Đ�
				//	m_SrcBgm3D[sourceVoiceNum].m_SrcVoice->SubmitSourceBuffer(&data.GetBuffer());

				m_SrcSe3D[sourceVoiceNum].m_SrcVoice->SetFrequencyRatio(m_SrcSe3D[sourceVoiceNum].m_DSPSettings.DopplerFactor);

				//�s��

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
		// �g�p�ł���Đ����\�[�X������
		int sourceVoiceNum = SearchSourceVoice(m_SrcSe3D.data(), m_SrcSe3D.size());
		if (sourceVoiceNum == -1)
		{
			return -1;
		}
		printf("se[%d]", sourceVoiceNum);
		//�G�~�b�^�[�̐ݒ�
		m_SrcSe3D[sourceVoiceNum].m_pos = pos;
		DirectX::XMFLOAT3 _pos = *m_SrcSe3D[sourceVoiceNum].m_pos;
		m_SrcSe3D[sourceVoiceNum].m_Emitter.Position = { _pos.x, _pos.y,_pos.z };
		m_SrcSe3D[sourceVoiceNum].m_Emitter.OrientFront = { 0.0f, 0.0f, 1.0f };
		m_SrcSe3D[sourceVoiceNum].m_Emitter.OrientTop = { 0.0f, 1.0f, 0.0f };
		m_SrcSe3D[sourceVoiceNum].m_Emitter.Velocity = { 0.0f, 0.0f, 0.0f };
		m_SrcSe3D[sourceVoiceNum].m_Emitter.pChannelAzimuths = new float[2] { -X3DAUDIO_PI / 2, X3DAUDIO_PI / 2 };	// �G�~�b�^�[�̕��ʊp
		m_SrcSe3D[sourceVoiceNum].m_Emitter.ChannelCount = data.GetChannelCount(); // �`�����l����
		m_SrcSe3D[sourceVoiceNum].m_Emitter.ChannelRadius = 1.0f; // �`�����l�����a
		m_SrcSe3D[sourceVoiceNum].m_Emitter.CurveDistanceScaler = m_SrcSe3D[sourceVoiceNum].m_Emitter.DopplerScaler = 1.0f; // �����X�P�[���[
		//m_SrcSe3D[sourceVoiceNum].m_Emitter.pVolumeCurve = data.GetVolumeCurve(); // �����i�����ɂ�鉹�ʂ�

		//DSP�ݒ�
		m_SrcSe3D[sourceVoiceNum].m_DSPSettings = { 0 };
		m_SrcSe3D[sourceVoiceNum].m_DSPSettings.SrcChannelCount = m_SrcSe3D[sourceVoiceNum].m_Emitter.ChannelCount; // �G�~�b�^�[�̃`�����l����
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

		//���ʂ̐ݒ�
		m_SrcSe3D[sourceVoiceNum].m_SrcVoice->SetVolume(data.GetVol());

		// 3D�I�[�f�B�I�v�Z
		X3DAudioCalculate(m_X3DInstance, &m_Listener, &m_SrcSe3D[sourceVoiceNum].m_Emitter, flag, &m_SrcSe3D[sourceVoiceNum].m_DSPSettings);

		// �\�[�X�{�C�X�̍Đ�

		m_SrcSe3D[sourceVoiceNum].m_SrcVoice->SetFrequencyRatio(m_SrcSe3D[sourceVoiceNum].m_DSPSettings.DopplerFactor);

		//�s��

		m_SrcSe3D[sourceVoiceNum].m_SrcVoice->SetOutputMatrix(m_MasterVoice, m_SrcSe3D[sourceVoiceNum].m_DSPSettings.SrcChannelCount, m_SrcSe3D[sourceVoiceNum].m_DSPSettings.DstChannelCount, m_SrcSe3D[sourceVoiceNum].m_DSPSettings.pMatrixCoefficients);
		m_SrcSe3D[sourceVoiceNum].m_SrcVoice->SubmitSourceBuffer(&data.GetBuffer());

		//�Đ�
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
		// �g�p�ł���Đ����\�[�X������
		int sourceVoiceNum = SearchSourceVoice(m_SrcSe2D.data(), m_SrcSe2D.size());
		if (sourceVoiceNum == -1)
		{
			return;
		}
		//��~
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
		// �g�p�ł���Đ����\�[�X������
		int sourceVoiceNum = SearchSourceVoice(m_SrcSe2D.data(), m_SrcSe2D.size());
		if (sourceVoiceNum == -1)
		{
			return;
		}
		//��~
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
		// �g�p�ł���Đ����\�[�X������
		int sourceVoiceNum = SearchSourceVoice(m_SrcBgm3D.data(), m_SrcBgm3D.size());
		if (sourceVoiceNum == -1)
		{
			return;
		}
		//��~
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
		// �g�p�ł���Đ����\�[�X������
		int sourceVoiceNum = SearchSourceVoice(m_SrcBgm3D.data(), m_SrcBgm3D.size());
		if (sourceVoiceNum == -1)
		{
			return;
		}
		//��~
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
		// �g�p�ł���Đ����\�[�X������
		int sourceVoiceNum = SearchSourceVoice(m_SrcSe3D.data(), m_SrcSe3D.size());
		if (sourceVoiceNum == -1)
		{
			return;
		}
		//��~
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
		// �g�p�ł���Đ����\�[�X������
		int sourceVoiceNum = SearchSourceVoice(m_SrcSe3D.data(), m_SrcSe3D.size());
		if (sourceVoiceNum == -1)
		{
			return;
		}
		//��~
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
		// �g�p�ł���Đ����\�[�X������
		int sourceVoiceNum = SearchSourceVoice(m_SrcBgm3D.data(), m_SrcBgm3D.size());
		if (sourceVoiceNum == -1)
		{
			return;
		}
		//�G�~�b�^�[�̐ݒ�
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
		// �g�p�ł���Đ����\�[�X������
		int sourceVoiceNum = SearchSourceVoice(m_SrcBgm3D.data(), m_SrcBgm3D.size());
		if (sourceVoiceNum == -1)
		{
			return -1;
		}
		//�G�~�b�^�[�̐ݒ�
		m_SrcBgm3D[sourceVoiceNum].m_pos = pos;
		DirectX::XMFLOAT3 _pos = *m_SrcBgm3D[sourceVoiceNum].m_pos;
		m_SrcBgm3D[sourceVoiceNum].m_Emitter.Position = { _pos.x, _pos.y,_pos.z };
		m_SrcBgm3D[sourceVoiceNum].m_Emitter.OrientFront = { 0.0f, 0.0f, 1.0f };
		m_SrcBgm3D[sourceVoiceNum].m_Emitter.OrientTop = { 0.0f, 1.0f, 0.0f };
		m_SrcBgm3D[sourceVoiceNum].m_Emitter.Velocity = { 0.0f, 0.0f, 0.0f };
		m_SrcBgm3D[sourceVoiceNum].m_Emitter.pChannelAzimuths = new float[2] { -X3DAUDIO_PI / 2, X3DAUDIO_PI / 2 };	// �G�~�b�^�[�̕��ʊp
		m_SrcBgm3D[sourceVoiceNum].m_Emitter.ChannelCount = data.GetChannelCount(); // �`�����l����
		m_SrcBgm3D[sourceVoiceNum].m_Emitter.ChannelRadius = 1.0f; // �`�����l�����a
		m_SrcBgm3D[sourceVoiceNum].m_Emitter.CurveDistanceScaler = m_SrcBgm3D[sourceVoiceNum].m_Emitter.DopplerScaler = 1.0f; // �����X�P�[���[
		//m_SrcBgm3D[sourceVoiceNum].m_Emitter.pVolumeCurve = data.GetVolumeCurve(); // �����i�����ɂ�鉹�ʂ̕ω��j�̐ݒ�

		//DSP�ݒ�
		m_SrcBgm3D[sourceVoiceNum].m_DSPSettings = { 0 };
		m_SrcBgm3D[sourceVoiceNum].m_DSPSettings.SrcChannelCount = m_SrcBgm3D[sourceVoiceNum].m_Emitter.ChannelCount; // �G�~�b�^�[�̃`�����l����
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

		//���ʂ̐ݒ�
		m_SrcBgm3D[sourceVoiceNum].m_SrcVoice->SetVolume(data.GetVol());

		// 3D�I�[�f�B�I�v�Z
		X3DAudioCalculate(m_X3DInstance, &m_Listener, &m_SrcBgm3D[sourceVoiceNum].m_Emitter, flag, &m_SrcBgm3D[sourceVoiceNum].m_DSPSettings);

		// �\�[�X�{�C�X�̍Đ�

		m_SrcBgm3D[sourceVoiceNum].m_SrcVoice->SetFrequencyRatio(m_SrcBgm3D[sourceVoiceNum].m_DSPSettings.DopplerFactor);

		//�s��

		m_SrcBgm3D[sourceVoiceNum].m_SrcVoice->SetOutputMatrix(m_MasterVoice, m_SrcBgm3D[sourceVoiceNum].m_DSPSettings.SrcChannelCount, m_SrcBgm3D[sourceVoiceNum].m_DSPSettings.DstChannelCount, m_SrcBgm3D[sourceVoiceNum].m_DSPSettings.pMatrixCoefficients);
		m_SrcBgm3D[sourceVoiceNum].m_SrcVoice->SubmitSourceBuffer(&data.GetBuffer());

		//�Đ�
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
	// �����̃\�[�X�{�C�X����g�p�ł��郊�\�[�X������
	//===================================================
	int SearchSourceVoice(IXAudio2SourceVoice **SourceVoice, int size)
	{
		// ����Đ����郊�\�[�X
		int sourceVoiceNum = -1;

		// ���\�[�X�̃o�b�t�@
		unsigned int soundBuffer = 0;

		// �g�p�ł���Đ����\�[�X������
		for (int i = 0; i < size; i++)
		{
			// ���݂̏�Ԃ��擾
			XAUDIO2_VOICE_STATE state;
			SourceVoice[i]->GetState(&state);

			// �o�b�t�@��0�Ȃ�΍Đ��\�Ɣ��f
			if (state.BuffersQueued == 0)
			{
				sourceVoiceNum = i;
				break;
			}
			else
			{
				// �����l�������̓o�b�t�@���ŏ��̏ꍇ�͓���ւ�
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
		// ����Đ����郊�\�[�X
		int sourceVoiceNum = -1;

		// ���\�[�X�̃o�b�t�@
		unsigned int soundBuffer = 0;

		// �g�p�ł���Đ����\�[�X������
		for (int i = 0; i < size; i++)
		{
			// ���݂̏�Ԃ��擾
			XAUDIO2_VOICE_STATE state;
			Sourcedata[i].m_SrcVoice->GetState(&state);

			// �o�b�t�@��0�Ȃ�΍Đ��\�Ɣ��f
			if (state.BuffersQueued == 0)
			{
				sourceVoiceNum = i;
				break;
			}
			else
			{
				// �����l�������̓o�b�t�@���ŏ��̏ꍇ�͓���ւ�
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
