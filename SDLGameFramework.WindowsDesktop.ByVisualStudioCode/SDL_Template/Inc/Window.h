#pragma once

#include <SDL.h>
#include <string>
#include "Vector2.h"
#include "Color.h"
#include "Texture.h"
#include "Rectangle.h"

class Window
{
private:
	// �������
	std::string mTitle;
	// �����С
	Vector2 mWinSize;
	// ����SDL����
	SDL_Window* mSDLWindow = nullptr;
	// �ô����SDL��Ⱦ��
	SDL_Renderer* mSDLRenderer = nullptr;

	// ����һ������
	void CreateWindow();

public:
	// ���캯�����������ڣ�
	Window(std::string title, Vector2 size);
	// ���� �� ��ֵ
	Window(const Window& other);
	Window(Window&& other)  noexcept;
	Window& operator= (const Window& other);
	Window& operator= (Window&& other)  noexcept;
	// �������������ٴ��ڣ�
	~Window();

	Vector2 GetSize();
	SDL_Renderer* GetSDLRenderer() { return mSDLRenderer; }

	void SetPenColor(Color color);
	void DrawPoint(Vector2 pos);
	void DrawLine(Vector2 pos1, Vector2 pos2);
	void DrawRect(Vector2 pos, Vector2 size);
	void DrawTexture(Texture& tex, 
		Rectangle srcRect = Rectangle::Zero, Rectangle dstRect = Rectangle::Zero, 
		double angle = 0.0, bool flipX = false, bool flipY = false);

	Texture CreateTextureForRender(Vector2 size);
	Texture CreateTextureForImage(Vector2 size);
	Texture LoadImage(std::string path);
	void SetRenderTarget(Texture& texture);
	void SetRenderTargetForWin();

	bool IsMinimized();

	void Clean();
	void Present();
};

