#include "RectTransform.h"
#include "GameObject.h"

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
}

void RectTransform::Awake()
{
    mRectTrans = ComponentPtr<RectTransform>(mSelfPtr);
}
