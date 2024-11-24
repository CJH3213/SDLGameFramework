#pragma once
#include "Scene.h"
#include "AudioManager.h"
#include "CanvasPF.h"

class TetrisGameScene :
    public Scene
{
private:
    std::shared_ptr<AudioPackage> mWav1;
    //std::shared_ptr<AudioPackage> mWav2;

public:
    virtual void Load() override;
};

class ScoreTextPF : public CanvasPrefab
{
public:virtual void Load() override;
};

class NextTextPF : public CanvasPrefab
{
public:virtual void Load() override;
};

class NextShapePF : public CanvasPrefab
{
public:virtual void Load() override;
};
