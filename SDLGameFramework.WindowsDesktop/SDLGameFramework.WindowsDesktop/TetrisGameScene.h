#pragma once
#include "Scene.h"
#include "AudioManager.h"

class TetrisGameScene :
    public Scene
{
private:
    std::shared_ptr<AudioPackage> mWav1;

public:
    virtual void Load() override;
};

