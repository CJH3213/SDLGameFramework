#pragma once
#include <vector>
#include "Window.h"

class WindowsManager
{
private:
	// ����ģʽ�£����ù���������ֵ
	WindowsManager() {};
	~WindowsManager() = default;
	WindowsManager(const WindowsManager&) = delete;
	WindowsManager& operator=(const WindowsManager&) = delete;
	// ��ȡ�����൥��
	static WindowsManager& GetInstance();

private:
	std::vector<Window*> mWindows;

public:
	static Window& Get(size_t i);
	static void Add(Window* win);
	static void Remove(Window* win);

	static void ReRenderTargetsCallback();
};

