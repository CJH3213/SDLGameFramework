#pragma once
#include "Transform.h"
#include "Rectangle.h"

class RectTransform :
    public Transform
{
private:
    ComponentPtr<RectTransform> mRectTrans;

public:
    Vector2 mAnchorMin, mAnchorMax;     // 基于父UI的归一化缩放大小
    Vector2 mAnchoredPosition;  // UI矩形相对于Anchor矩形的位置偏移量
    Vector2 mSizeDelta; // UI矩形相对于Anchored矩形的大小变化量
    Vector2 mPivot;     // UI矩形与Ancored矩形的对齐比例

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
    // 获取世界坐标下的矩形
    void GetLocalCorners(Vector2 fourCornersArray[]);
    void GetWorldCorners(Vector2 fourCornersArray[]);
    void SetSizeWithCurrentAnchors(Axis axis, float size);

    virtual void Awake() override;
};