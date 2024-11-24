#pragma once
#include "Scene.h"
#include "AudioManager.h"

class TestScene :
    public Scene
{
private:
    std::shared_ptr<AudioPackage> mWav1;

public:
    virtual void Load() override;
};

