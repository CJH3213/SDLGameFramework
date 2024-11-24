#pragma once
#include "Component.h"

class ShapeScript :
    public Component
{
private:

public:
    //ComponentPtr<TetrisManager> tetrisManagerSC;

    virtual void Awake() override;
    virtual void Update() override;

};

