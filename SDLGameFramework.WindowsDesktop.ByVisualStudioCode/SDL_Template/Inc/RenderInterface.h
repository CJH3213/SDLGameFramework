#pragma once
#include "Camera.h"

struct RenderInterface
{
    virtual int GetOrder();
    virtual void Draw(Camera& cam);
};

