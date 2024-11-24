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
	// 窗体标题
	std::string mTitle;
	// 窗体大小
	Vector2 mWinSize;
	// 窗体SDL对象
	SDL_Window* mSDLWindow = nullptr;
	// 该窗体的SDL渲染器
	SDL_Renderer* mSDLRenderer = nullptr;

	// 创建一个窗体
	void CreateWindow();

public:
	// 构造函数（创建窗口）
	Window(std::string title, Vector2 size);
	// 构造 和 赋值
	Window(const Window& other);
	Window(Window&& other)  noexcept;
	Window& operator= (const Window& other);
	Window& operator= (Window&& other)  noexcept;
	// 析构函数（销毁窗口）
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

