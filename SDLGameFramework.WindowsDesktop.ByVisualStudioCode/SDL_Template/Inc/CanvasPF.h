#pragma once
#include "Prefab.h"

// Canvas游戏对象继承此类
class CanvasPrefab : public Prefab
{
public:virtual void Start() override;
};

class CanvasPF : public CanvasPrefab
{
public:virtual void Load() override;
};

