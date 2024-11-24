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
	// SDL音频信息
	SDL_AudioSpec mDesiredSpec;
	// 是否循环播放
	bool mIsLoop = false;
	// 当前是否正在播放
	bool mIsPlaying = false;
	// 音量
	float mVolume = 1.0f;
	// 当前播放进度
	size_t mPlaybackProgress = 0;
	// 音频数据
	std::vector<uint8_t> mData;
	// 封面图片
	Texture mCoverImage;

	// 注意音频数据非常大可达几MB以上，
	// 不要直接复制AudioPackage，使用智能指针操作它

	// 是否播放完毕（不是暂停）
	bool IsFinish() const { return mPlaybackProgress >= mData.size(); }
	// 获取持续时间（单位：秒）
	double GetDuration() const;
};

class AudioManager
{
private:
	// 单例模式下，禁用构造析构赋值
	AudioManager();
	~AudioManager() = default;
	AudioManager(const AudioManager&) = delete;
	AudioManager& operator=(const AudioManager&) = delete;
	// 获取输入类单例
	static AudioManager& GetInstance();

	// 初始化SDL音频设备
	SDL_AudioSpec mDesiredSpec;
	// 音频数据包容器
	std::vector<std::weak_ptr<AudioPackage>> mAudioPackages;
	// 访问容器互斥锁
	std::mutex mMutex;

	// SDL音频流回调
	static void AudioCallback(void* userdata, uint8_t* stream, int len);

public:
	static void Init();
	static void Close();

	// 播放音频
	static void Play(std::weak_ptr<AudioPackage> pack_wp);
	// 手动移除音频
	static void Remove(AudioPackage pack);

	// 加载WAV音频文件
	static std::shared_ptr<AudioPackage> LoadWav(std::string path);
	// 加载任意格式音频文件（能被FFMpeg解码的格式）
	static std::shared_ptr<AudioPackage> LoadAudioFile(std::string path);
};


