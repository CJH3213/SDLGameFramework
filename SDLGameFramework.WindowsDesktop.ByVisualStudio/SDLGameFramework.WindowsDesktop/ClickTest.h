#pragma once
#include "Component.h"
#include "BoxCollider.h"

class ClickTest :
    public Component
{
private:
    ComponentPtr<BoxCollider> mColliderCom;

    void OnMouseDown();
    void OnMouseStay();
    void OnMouseUp();
    void OnClick();

public:
    virtual void Awake() override;
    virtual void Update() override;
};

