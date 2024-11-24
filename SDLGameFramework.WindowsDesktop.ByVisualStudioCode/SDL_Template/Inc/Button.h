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

    // ��ť��ͨͼƬ
    Texture mNormalTex;
    // ��ť�߹�ͼƬ
    Texture mHighLigTex;
    // ��ť��ѹͼƬ
    Texture mPressedTex;
    // ����ȾĿ������
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

