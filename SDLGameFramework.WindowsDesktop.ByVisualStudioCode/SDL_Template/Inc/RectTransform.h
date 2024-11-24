#pragma once
#include "Transform.h"
#include "Rectangle.h"

class RectTransform :
    public Transform
{
private:
    ComponentPtr<RectTransform> mRectTrans;

    Vector2 mAnchorMin, mAnchorMax;     // 基于父UI的归一化缩放大小
    Vector2 mAnchoredPosition;  // UI矩形相对于Anchor矩形的位置偏移量
    Vector2 mSizeDelta; // UI矩形相对于Anchored矩形的大小变化量
    Vector2 mPivot;     // UI矩形与Ancored矩形的对齐比例

public:

    enum Edge
    {
        Left, Right, Top, Bottom
    };

    enum Axis
    {
        Horizontal, Vertical
    };

    Vector2 GetAnchorMin() const { return mAnchorMin; }
    void SetAnchorMin(Vector2 v2);

    Vector2 GetAnchorMax() const { return mAnchorMax; }
    void SetAnchorMax(Vector2 v2);

    Vector2 GetAnchoredPosition() const { return mAnchoredPosition; }
    void SetAnchoredPosition(Vector2 v2);

    Vector2 GetSizeDelta() const { return mSizeDelta; }
    void SetSizeDelta(Vector2 v2);

    Vector2 GetPivot() const { return mPivot; }
    void SetPivot(Vector2 v2);

    Rectangle GetParentRect();
    // 获得当前UI缩放约束后的矩形
    Rectangle GetAnchoredRect();
    // 获得当前UI矩形（大小，世界坐标）
    Rectangle GetRectangle();
    // 获取4个角点相对坐标
    void GetLocalCorners(Vector2 fourCornersArray[]);
    // 获得4个角点世界坐标
    void GetWorldCorners(Vector2 fourCornersArray[]);
    // 在当前缩放约束条件下，根据轴和大小设置
    void SetSizeWithCurrentAnchors(Axis axis, float size);
    // 与父UI的边界、距离、当前UI大小设置
    void SetInsetAndSizeFromParentEdge(Edge edge, float inset, float size);

    virtual void Awake() override;
    virtual void Update() override;
};