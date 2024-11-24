#include "MusicPlayScript.h"
#include <filesystem>
#include <iostream>
#include "Debug.h"
#include "AudioManager.h"
#include "Input.h"
#include "ImagesManager.h"

// ͨ����׺�ж��Ƿ�Ϊ֧�ֵ���Ƶ�ļ���ʽ
bool IsMusicExtension(const std::filesystem::path& extPath)
{
	return extPath == std::filesystem::path(".wav")
		|| extPath == std::filesystem::path(".mp3")
		|| extPath == std::filesystem::path(".ape")
		|| extPath == std::filesystem::path(".flac");
}

// ���ļ����»�ȡ������Ƶ�ļ���·��
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
		// path������std::cout����ȽϷ���
		// �����ʹ��print�������Ҫ�ȴ�path�л�ȡwstring�����ٴ�string��ȡwchar*
		// ͬʱ����Ҫע���ʽ����Ϊ���ַ���wchar��%S��ʽ��ʶ
	}

	return musicPaths;
}

// ��ӡ��Ƶ�ĳ���ʱ��
void PrintDuration(double sec)
{
	double decimal = sec - (int)sec;	// std::modf(sec, &integer);  // ȡС������
	std::cout << "����ʱ�䣺" << sec << "s = " << (int)sec / 60 << ':' 
		<< (int)sec % 60 << '.' << (int)((decimal+0.005)*100) << '\n';
}

// �л�����ָ��·������Ƶ
bool MusicPlayScript::PlayMusicByDir(std::string dirPath)
{
	// ͨ���ļ�·�������������ݰ�
	mWav1 = AudioManager::LoadAudioFile(dirPath);

	// ��������ļ����سɹ�
	if (mWav1)
	{
		AudioManager::Play(mWav1);		// ����
		PrintDuration(mWav1->GetDuration());	// �����Ƶ��Ϣ���ն�
		//	��������
		Texture tex = mWav1->mCoverImage ? mWav1->mCoverImage : mNoCoverImg;
		tex.SetWidth(300);
		(*mCoverImage).SetTexture(tex);
	}

	// ���أ���Ƶ�Ƿ�ɹ�����
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
	// �����ǰ�����������
	if (!mWav1 || mWav1->IsFinish())
	{
		mWav1 = nullptr;

		for(; mIt_MusicPaths != mMusicPaths.end();)
			if (PlayMusicByDir(*mIt_MusicPaths++))
				break;
	}

	// �ո����ͣ��ǰ����
	if (Input::GetKeyDown(SDL_SCANCODE_SPACE))
		if(mWav1) mWav1->mIsPlaying = !mWav1->mIsPlaying;

	// ��һ��
	if (Input::GetKeyDown(SDL_SCANCODE_RIGHT))
	{
		// ���һ���󣬻ص���һ��
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
