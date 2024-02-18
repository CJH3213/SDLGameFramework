#pragma once
#include <SDL_audio.h>
#include <vector>
#include <mutex>

class AudioPackage
{
public:
	// SDL��Ƶ��Ϣ
	SDL_AudioSpec mDesiredSpec;
	// �Ƿ�ѭ������
	bool mIsLoop = false;
	// ����
	float mVolume = 1.0f;
	// ��ǰ���Ž���
	size_t mPlaybackProgress = 0;
	// ��Ƶ����
	std::vector<uint8_t> mData;

	// ע����Ƶ���ݷǳ���ɴＸMB���ϣ�
	// ��Ҫֱ�Ӹ���AudioPackage��ʹ������ָ�������
};

class AudioManager
{
private:
	// ����ģʽ�£����ù���������ֵ
	AudioManager();
	~AudioManager() = default;
	AudioManager(const AudioManager&) = delete;
	AudioManager& operator=(const AudioManager&) = delete;
	// ��ȡ�����൥��
	static AudioManager& GetInstance();

	// ��ʼ��SDL��Ƶ�豸
	SDL_AudioSpec mDesiredSpec;
	// ��Ƶ���ݰ�����
	std::vector<std::weak_ptr<AudioPackage>> mAudioPackages;
	// ��������������
	std::mutex mMutex;

	// SDL��Ƶ���ص�
	static void AudioCallback(void* userdata, uint8_t* stream, int len);

public:
	static void Init();
	static void Close();

	// ������Ƶ
	static void Play(std::weak_ptr<AudioPackage> pack_wp);
	// �ֶ��Ƴ���Ƶ
	static void Remove(AudioPackage pack);

	// ����WAV��Ƶ�ļ�
	static std::shared_ptr<AudioPackage> LoadWav(std::string path);
	// ����MP3��Ƶ�ļ�
	static std::shared_ptr<AudioPackage> LoadAudioFile(std::string path);
};


