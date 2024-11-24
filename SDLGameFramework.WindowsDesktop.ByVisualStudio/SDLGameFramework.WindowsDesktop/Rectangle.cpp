#include "Rectangle.h"

const Rectangle Rectangle::Zero = { Vector2::Zero, Vector2::Zero };

//bool Rectangle::operator==(bool other) const
//{
//    return !(mCenterPos == Vector2::Zero && mSize == Size::Zero);
//}

bool Rectangle::operator==(const Rectangle& other) const
{
    return mCenterPos == other.mCenterPos && mSize == other.mSize;
}

Rectangle::operator SDL_Rect() const
{
    // 注意SDL矩形左上角y值小，游戏坐标中矩形左上角y值大，二者颠倒
    Vector2 leftTopPos = GetLeftBottomPos();
    return 
    {
        static_cast<int>(leftTopPos.X),
        static_cast<int>(leftTopPos.Y),
        static_cast<int>(mSize.X),
        static_cast<int>(mSize.Y)
    };
}

Rectangle::operator SDL_FRect() const
{
    // 注意SDL矩形左上角y值小，游戏坐标中矩形左上角y值大，二者颠倒
    Vector2 leftTopPos = GetLeftBottomPos();
    return
    {
        leftTopPos.X, leftTopPos.Y,
        mSize.X, mSize.Y
    };
}

Vector2 Rectangle::GetLeftTopPos() const
{
    return { mCenterPos.X - mSize.X/2, mCenterPos.Y + mSize.Y/2};
}

Vector2 Rectangle::GetLeftBottomPos() const
{
    return { mCenterPos.X - mSize.X / 2, mCenterPos.Y - mSize.Y / 2 };
}

Vector2 Rectangle::GetRightTopPos() const
{
    return { mCenterPos.X + mSize.X / 2, mCenterPos.Y + mSize.Y / 2 };
}

Vector2 Rectangle::GetRightBottomPos() const
{
    return { mCenterPos.X + mSize.X / 2, mCenterPos.Y - mSize.Y / 2 };
}

Vector2 Rectangle::GetLeftCenterPos() const
{
    return { mCenterPos.X - mSize.X / 2, mCenterPos.Y};
}

Vector2 Rectangle::GetRightCenterPos() const
{
    return { mCenterPos.X + mSize.X / 2, mCenterPos.Y};
}

Vector2 Rectangle::GetTopCenterPos() const
{
    return { mCenterPos.X, mCenterPos.Y + mSize.Y / 2 };
}

Vector2 Rectangle::GetBottomCenterPos() const
{
    return { mCenterPos.X, mCenterPos.Y - mSize.Y / 2 };
}

bool Rectangle::IsIntersect(const Rectangle& rect) const
{
    Vector2 blp1 = GetLeftBottomPos();
    Vector2 trp1 = GetRightTopPos();
    Vector2 blp2 = rect.GetLeftBottomPos();
    Vector2 trp2 = rect.GetRightTopPos();

    return Math::Max(blp1.X, blp2.X) <= Math::Min(trp1.X, trp2.X) &&
        Math::Max(blp1.Y, blp2.Y) <= Math::Min(trp1.Y, trp2.Y);
}

bool Rectangle::IsInside(const Rectangle& rect) const
{
    Vector2 blp1 = GetLeftBottomPos();
    Vector2 trp1 = GetRightTopPos();
    Vector2 blp2 = rect.GetLeftBottomPos();
    Vector2 trp2 = rect.GetRightTopPos();

    return blp1.X >= blp2.X && blp1.Y >= blp2.Y &&
        trp1.X <= trp2.X && trp1.Y <= trp2.Y;
}

bool Rectangle::IsSame(const Rectangle& rect) const
{
    return (mCenterPos == rect.mCenterPos) &&
        (mSize == rect.mSize);
}

bool Rectangle::IsIntersect(const Vector2& pos) const
{
    Vector2 tlp = GetLeftTopPos();
    Vector2 brp = GetRightBottomPos();

    // 要大于等于左上角点且小于不等于右下角点
    return pos.X >= tlp.X && pos.X < brp.X &&
        pos.Y <= tlp.Y && pos.Y > brp.Y;
}
