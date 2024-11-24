#pragma once

extern "C"
{
	#include <SDL.h>
}

#include "Vector2.h"

class Rectangle
{
private:

public:
	Vector2 mCenterPos;
	Vector2 mSize;

	static const Rectangle Zero;

	Rectangle() : mCenterPos(Vector2::Zero), mSize(Vector2::Zero) {};
	Rectangle(Vector2 _v, Vector2 _s) :mCenterPos(_v), mSize(_s.X, _s.Y) {};

	bool operator==(const Rectangle& other) const;
	operator SDL_Rect() const;
	operator SDL_FRect() const;

	Vector2 GetLeftTopPos() const;
	Vector2 GetLeftBottomPos() const;
	Vector2 GetRightTopPos() const;
	Vector2 GetRightBottomPos() const;
	Vector2 GetLeftCenterPos() const;
	Vector2 GetRightCenterPos() const;
	Vector2 GetTopCenterPos() const;
	Vector2 GetBottomCenterPos() const;

	// �������Ƿ��ཻ
	bool IsIntersect(const Rectangle& rect) const;
	// ��ǰ���� �Ƿ񱻰����� ��һ���� ��
	bool IsInside(const Rectangle& rect) const;
	// �Ƿ���ͬ
	bool IsSame(const Rectangle& rect) const;

	// �����Ƿ�����ཻ
	bool IsIntersect(const Vector2& pos) const;
};

