#pragma once
#include <SDL.h>

struct Color
{
	unsigned char A;
	unsigned char R;
	unsigned char G;
	unsigned char B;

	// ��ʽת��SDL_Color
	inline operator SDL_Color() const
	{
		return { R, G, B, A };
	}

	// �����ǳ�����ɫ
	static const Color Black;
	static const Color White;
	static const Color Red;
	static const Color Green;
	static const Color Blue;
	static const Color Orange;
};

