#pragma once
#include "Transform.h"
#include "Rectangle.h"

class RectTransform :
    public Transform
{
private:
    ComponentPtr<RectTransform> mRectTrans;

    Vector2 mAnchorMin, mAnchorMax;     // ���ڸ�UI�Ĺ�һ�����Ŵ�С
    Vector2 mAnchoredPosition;  // UI���������Anchor���ε�λ��ƫ����
    Vector2 mSizeDelta; // UI���������Anchored���εĴ�С�仯��
    Vector2 mPivot;     // UI������Ancored���εĶ������

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
    // ��õ�ǰUI����Լ����ľ���
    Rectangle GetAnchoredRect();
    // ��õ�ǰUI���Σ���С���������꣩
    Rectangle GetRectangle();
    // ��ȡ4���ǵ��������
    void GetLocalCorners(Vector2 fourCornersArray[]);
    // ���4���ǵ���������
    void GetWorldCorners(Vector2 fourCornersArray[]);
    // �ڵ�ǰ����Լ�������£�������ʹ�С����
    void SetSizeWithCurrentAnchors(Axis axis, float size);
    // �븸UI�ı߽硢���롢��ǰUI��С����
    void SetInsetAndSizeFromParentEdge(Edge edge, float inset, float size);

    virtual void Awake() override;
    virtual void Update() override;
};