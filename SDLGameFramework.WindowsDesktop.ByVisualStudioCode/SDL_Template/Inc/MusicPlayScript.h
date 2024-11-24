#pragma once
#include "Component.h"
#include "Prefab.h"
#include "AudioManager.h"
#include "Image.h"

class MusicPlayScript :
    public Component
{
private:
    ComponentPtr<Image> mCoverImage;

    std::vector<std::string> mMusicPaths;
    std::vector<std::string>::iterator mIt_MusicPaths;
    std::shared_ptr<AudioPackage> mWav1;
    Texture mNoCoverImg;

    // 切换播放指定路径的音频
    bool PlayMusicByDir(std::string dirPath);

public:
    virtual void Awake() override;
    virtual void Start() override;
    virtual void Update() override;
};

class MusicPlayPF : public Prefab
{
public:virtual void Load() override;
};
