#pragma once

extern "C"
{
	#include <SDL.h>
}

#include "Rectangle.h"
#include "Vector2.h"
#include <memory>
#include <string>


class Texture
{
private:
	struct _SDL_Texture
	{
		SDL_Texture* mSDLTexture = nullptr;
		~_SDL_Texture()
		{
			SDL_DestroyTexture(mSDLTexture);
			mSDLTexture = nullptr;
		}
		SDL_Texture* operator*() const
		{
			return mSDLTexture;
		}
	};
	std::shared_ptr<_SDL_Texture> mSDLTexture_SP;

public:
	Vector2 mSize;	// �������ʾ��С
	Rectangle mSliceRect;	// ������ʾ��ȡ���򣬴����½ǿ�ʼ��
	Vector2 mAnchorPos;		// ê��

	Texture();
	Texture(SDL_Texture* tex, Rectangle slice = Rectangle::Zero, Vector2 anchor = Vector2::Zero);
	~Texture();

	// ����SDL�����ڲ�ʹ�ã��û�������Ҫ���ã�
	SDL_Texture* GetSDLTexture();
	// ��ȡ����ԭʼ��С
	Vector2 GetOriginalSize();
	// ����ͼƬ��ȣ��߶Ȱ������ı�
	void SetWidth(float w);
	// ����ͼƬ�߶�, ��Ȱ������ı�
	void SetHeight(float h);

	// ��ǰ�����Ƿ�������ͼ
	operator bool() const noexcept;
};

