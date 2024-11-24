#pragma once

extern "C"
{
	#include <SDL_audio.h>
}

#include <vector>
#include <mutex>
#include "Texture.h"

class AudioPackage
{
public:
	// SDL��Ƶ��Ϣ
	SDL_AudioSpec mDesiredSpec;
	// �Ƿ�ѭ������
	bool mIsLoop = false;
	// ��ǰ�Ƿ����ڲ���
	bool mIsPlaying = false;
	// ����
	float mVolume = 1.0f;
	// ��ǰ���Ž���
	size_t mPlaybackProgress = 0;
	// ��Ƶ����
	std::vector<uint8_t> mData;
	// ����ͼƬ
	Texture mCoverImage;

	// ע����Ƶ���ݷǳ���ɴＸMB���ϣ�
	// ��Ҫֱ�Ӹ���AudioPackage��ʹ������ָ�������

	// �Ƿ񲥷���ϣ�������ͣ��
	bool IsFinish() const { return mPlaybackProgress >= mData.size(); }
	// ��ȡ����ʱ�䣨��λ���룩
	double GetDuration() const;
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
	// ���������ʽ��Ƶ�ļ����ܱ�FFMpeg����ĸ�ʽ��
	static std::shared_ptr<AudioPackage> LoadAudioFile(std::string path);
};


