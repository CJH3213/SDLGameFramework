#pragma once
#include "Transform.h"
#include "Rectangle.h"

class RectTransform :
    public Transform
{
private:
    ComponentPtr<RectTransform> mRectTrans;

public:
    Vector2 mAnchorMin, mAnchorMax;     // ���ڸ�UI�Ĺ�һ�����Ŵ�С
    Vector2 mAnchoredPosition;  // UI���������Anchor���ε�λ��ƫ����
    Vector2 mSizeDelta; // UI���������Anchored���εĴ�С�仯��
    Vector2 mPivot;     // UI������Ancored���εĶ������

    enum Edge
    {
        Left, Right, Top, Bottom
    };

    enum Axis
    {
        Horizontal, Vertical
    };

    Rectangle GetParentRect();
    Rectangle GetAnchoredRect();
    Rectangle GetRectangle();
    // ��ȡ���������µľ���
    void GetLocalCorners(Vector2 fourCornersArray[]);
    void GetWorldCorners(Vector2 fourCornersArray[]);
    void SetSizeWithCurrentAnchors(Axis axis, float size);

    virtual void Awake() override;
};