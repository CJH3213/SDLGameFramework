#include "RectTransform.h"
#include "GameObject.h"

void RectTransform::SetAnchorMin(Vector2 v2)
{
    if (mAnchorMin != v2)
    {
        mAnchorMin = v2;
    }
}

void RectTransform::SetAnchorMax(Vector2 v2)
{
    if (mAnchorMax != v2)
    {
        mAnchorMax = v2;
    }
}

void RectTransform::SetAnchoredPosition(Vector2 v2)
{
    if (mAnchoredPosition != v2)
    {
        mAnchoredPosition = v2;
    }
}

void RectTransform::SetSizeDelta(Vector2 v2)
{
    if (mSizeDelta != v2)
    {
        mSizeDelta = v2;
    }
}

void RectTransform::SetPivot(Vector2 v2)
{
    if (mPivot != v2)
    {
        mPivot = v2;
    }
}

Rectangle RectTransform::GetParentRect()
{
    if (mParentTrans != nullptr)
    {
        auto rtc = dynamic_cast<RectTransform*>(mParentTrans);
        if(rtc) return rtc->GetRectangle();
    }
    return Rectangle::Zero;
}

Rectangle RectTransform::GetAnchoredRect()
{
    auto parentRect = GetParentRect();
    Vector2 anchorSize = parentRect.mSize * (mAnchorMax - mAnchorMin);
    Vector2 anchorPos = parentRect.GetLeftBottomPos() + mAnchorMin * parentRect.mSize + anchorSize / 2;

    return {anchorPos, anchorSize};
}

Rectangle RectTransform::GetRectangle()
{
    Rectangle rect;

    Rectangle anchoredRect = GetAnchoredRect();
    Vector2 pivotPos = anchoredRect.GetLeftBottomPos() + mPivot * anchoredRect.mSize + mAnchoredPosition;
    rect.mSize = anchoredRect.mSize + mSizeDelta;
    rect.mCenterPos += pivotPos + (Vector2{0.5f, 0.5f} - mPivot) * rect.mSize;

    return rect;
}

void RectTransform::GetLocalCorners(Vector2 fourCornersArray[])
{
    // 左下角开始，顺时针
    auto rect = GetRectangle();
    fourCornersArray[0] = rect.GetLeftBottomPos();
    fourCornersArray[1] = rect.GetLeftTopPos();
    fourCornersArray[2] = rect.GetRightTopPos();
    fourCornersArray[3] = rect.GetRightBottomPos();
}

void RectTransform::GetWorldCorners(Vector2 fourCornersArray[])
{
    GetLocalCorners(fourCornersArray);
    Matrix4x4 matrix4x = GetTransform().LocalToWorldMatrix();
    for (int i = 0; i < 4; i++)
    {
        fourCornersArray[i] = Vector3( matrix4x * Vector3(fourCornersArray[i]));
    }
}

void RectTransform::SetSizeWithCurrentAnchors(Axis axis, float size)
{
    auto anRect = GetAnchoredRect();
    if (axis == Axis::Horizontal)
        SetSizeDelta({ size - anRect.mSize.X, mSizeDelta.Y });
    else
        SetSizeDelta({ mSizeDelta.X, size - anRect.mSize.Y});
}

void RectTransform::SetInsetAndSizeFromParentEdge(Edge edge, float inset, float size)
{
    switch (edge)
    {
    case RectTransform::Left:
        SetAnchorMin({ 0.0f, GetAnchorMin().Y });
        SetAnchorMax({ 0.0f, GetAnchorMax().Y });
        SetSizeDelta({ size, GetSizeDelta().Y });
        SetAnchoredPosition({ inset + size * GetPivot().X, GetAnchoredPosition().Y });
        break;
    case RectTransform::Right:
        SetAnchorMin({ 1.0f, GetAnchorMin().Y });
        SetAnchorMax({ 1.0f, GetAnchorMax().Y });
        SetSizeDelta({ size, GetSizeDelta().Y });
        SetAnchoredPosition({ -inset - size * (1.0f - GetPivot().X), GetAnchoredPosition().Y });
        break;
    case RectTransform::Top:
        SetAnchorMin({ GetAnchorMin().X, 1.0f });
        SetAnchorMax({ GetAnchorMax().X, 1.0f });
        SetSizeDelta({ GetSizeDelta().X, size });
        SetAnchoredPosition({ GetAnchoredPosition().X, -inset - size * (1.0f - GetPivot().Y) });
        break;
    case RectTransform::Bottom:
        SetAnchorMin({ GetAnchorMin().X, 0.0f });
        SetAnchorMax({ GetAnchorMax().X, 0.0f });
        SetSizeDelta({ GetSizeDelta().X, size });
        SetAnchoredPosition({ GetAnchoredPosition().X, inset + size * GetPivot().Y });
        break;
    default:
        break;
    }
}

void RectTransform::Awake()
{
    mRectTrans = ComponentPtr<RectTransform>(mSelfPtr);
}

void RectTransform::Update()
{
    if (mGameObject->mName == "NextShape")
    {
        bool b = 1;
    }
    auto parentRect = GetParentRect();
    auto rect = GetRectangle(); // 获得当前UI矩形（世界坐标）
    mLocalPosition = (rect.mCenterPos - parentRect.mCenterPos) + 
        rect.GetLeftBottomPos() - rect.mCenterPos + rect.mSize * mPivot;
}
