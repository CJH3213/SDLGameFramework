#pragma once
#include "Prefab.h"

// Canvas��Ϸ����̳д���
class CanvasPrefab : public Prefab
{
public:virtual void Start() override;
};

class CanvasPF : public CanvasPrefab
{
public:virtual void Load() override;
};

