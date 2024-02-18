#pragma once
#include <SDL.h>

struct Color
{
	unsigned char A;
	unsigned char R;
	unsigned char G;
	unsigned char B;

	// 隐式转换SDL_Color
	inline operator SDL_Color() const
	{
		return { R, G, B, A };
	}

	// 以下是常用颜色
	static const Color Black;
	static const Color White;
	static const Color Red;
	static const Color Green;
	static const Color Blue;
	static const Color Orange;
};

