#pragma once
#include "Component.h"
#include "RectTransform.h"
#include "Texture.h"
#include "CanvasRenderer.h"
#include "BoxCollider.h"

class Button :
    public Component
{
private:
    ComponentPtr<RectTransform> mRectTrans;
    ComponentPtr<CanvasRenderer> mRenderer;
    ComponentPtr<BoxCollider> mCollider;

    // 按钮普通图片
    Texture mNormalTex;
    // 按钮高光图片
    Texture mHighLigTex;
    // 按钮按压图片
    Texture mPressedTex;
    // 待渲染目标纹理
    Texture mRenderTex;

    bool mIsDirty = true;

    void OnMouseDown();
    void OnClick();

    void OnMouseEnter();
    void OnMouseExit();

public:

    void SetNormalTex(Texture tex);
    void SetmHighLigTex(Texture tex);
    void SetPressedTex(Texture tex);

    virtual void Awake() override;
    virtual void Update() override;
};

