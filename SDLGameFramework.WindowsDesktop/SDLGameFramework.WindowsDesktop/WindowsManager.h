#pragma once
#include <vector>
#include "Window.h"

class WindowsManager
{
private:
	// 单例模式下，禁用构造析构赋值
	WindowsManager() {};
	~WindowsManager() = default;
	WindowsManager(const WindowsManager&) = delete;
	WindowsManager& operator=(const WindowsManager&) = delete;
	// 获取输入类单例
	static WindowsManager& GetInstance();

private:
	std::vector<Window*> mWindows;

public:
	static Window& Get(size_t i);
	static void Add(Window* win);
	static void Remove(Window* win);

	static void ReRenderTargetsCallback();
};

