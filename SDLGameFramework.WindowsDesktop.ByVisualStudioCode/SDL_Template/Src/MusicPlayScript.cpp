#include "MusicPlayScript.h"
#include <filesystem>
#include <iostream>
#include "Debug.h"
#include "AudioManager.h"
#include "Input.h"
#include "ImagesManager.h"

// 通过后缀判断是否为支持的音频文件格式
bool IsMusicExtension(const std::filesystem::path& extPath)
{
	return extPath == std::filesystem::path(".wav")
		|| extPath == std::filesystem::path(".mp3")
		|| extPath == std::filesystem::path(".ape")
		|| extPath == std::filesystem::path(".flac");
}

// 从文件夹下获取所有音频文件的路径
std::vector<std::string> GetAllMusicPathInDir(std::string dirPath)
{
	std::vector<std::string> musicPaths;

	if (std::filesystem::exists(dirPath) == false)
		return musicPaths;

	for (const auto& entry : std::filesystem::directory_iterator(dirPath))
	{
		auto& path = entry.path();
		std::cout << "sc: " << path << '\n';
		Debug::Log(u8"  DBL: %S\n", path.extension().c_str());
		auto isMusicFile = IsMusicExtension(path.extension());
		if (isMusicFile)
			musicPaths.emplace_back(path.string());
		// path对象用std::cout输出比较方便
		// 如果是使用print输出，需要先从path中获取wstring对象，再从string获取wchar*
		// 同时还需要注意格式必须为宽字符型wchar，%S格式标识
	}

	return musicPaths;
}

// 打印音频的持续时间
void PrintDuration(double sec)
{
	double decimal = sec - (int)sec;	// std::modf(sec, &integer);  // 取小数部分
	std::cout << "持续时间：" << sec << "s = " << (int)sec / 60 << ':' 
		<< (int)sec % 60 << '.' << (int)((decimal+0.005)*100) << '\n';
}

// 切换播放指定路径的音频
bool MusicPlayScript::PlayMusicByDir(std::string dirPath)
{
	// 通过文件路径加载音乐数据包
	mWav1 = AudioManager::LoadAudioFile(dirPath);

	// 如果音乐文件加载成功
	if (mWav1)
	{
		AudioManager::Play(mWav1);		// 播放
		PrintDuration(mWav1->GetDuration());	// 输出音频信息到终端
		//	更换封面
		Texture tex = mWav1->mCoverImage ? mWav1->mCoverImage : mNoCoverImg;
		tex.SetWidth(300);
		(*mCoverImage).SetTexture(tex);
	}

	// 返回：音频是否成功加载
	return mWav1 != nullptr;
}

void MusicPlayScript::Awake()
{
	mMusicPaths = GetAllMusicPathInDir("Resource/Sounds/");
	mIt_MusicPaths = mMusicPaths.begin();
}

void MusicPlayScript::Start()
{
	mCoverImage = GameObject::Find("CoverImage")->GetComponent<Image>();
	mNoCoverImg = ImagesManager::ImageToTexture("Resource/Images/MusicIcon.jpeg");
	//(*mCoverImage).SetTexture(ImagesManager::ImageToTexture("Resource/Images/MusicIcon.jpeg"));
}

void MusicPlayScript::Update()
{
	// 如果当前歌曲播放完毕
	if (!mWav1 || mWav1->IsFinish())
	{
		mWav1 = nullptr;

		for(; mIt_MusicPaths != mMusicPaths.end();)
			if (PlayMusicByDir(*mIt_MusicPaths++))
				break;
	}

	// 空格键暂停当前音乐
	if (Input::GetKeyDown(SDL_SCANCODE_SPACE))
		if(mWav1) mWav1->mIsPlaying = !mWav1->mIsPlaying;

	// 下一首
	if (Input::GetKeyDown(SDL_SCANCODE_RIGHT))
	{
		// 最后一曲后，回到第一曲
		if (mIt_MusicPaths == mMusicPaths.end())
			mIt_MusicPaths = mMusicPaths.begin();
		PlayMusicByDir(*mIt_MusicPaths++);
	}
}

void MusicPlayPF::Load()
{
	auto com1 = std::make_shared<MusicPlayScript>();
	AddComponent(com1);
}
