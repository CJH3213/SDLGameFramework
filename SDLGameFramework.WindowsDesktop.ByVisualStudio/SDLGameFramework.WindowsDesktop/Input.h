#pragma once
#include "Event.h"
#include "Vector2.h"

class Input
{
private:
	// 单例模式下，禁用构造析构赋值
	Input();
	~Input() = default;
	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;

	// 事件集合
	Event<int, void()> mEvent;
	
	// 鼠标按钮状态
	bool mMouseBottons[3][2];
	// 鼠标坐标
	Vector2 mMousePos;
	// 键盘按钮状态
	bool mKeyboard[SDL_NUM_SCANCODES][2];

public:
	// 获取输入类单例
	static Input& GetInstance();

	/* 其他对外访问的函数均为static类型，间接访问单例 */

	// 处理窗口事件的、系统级更新
	static void SystemUpdate();
	// 每帧更新
	static void FrameUpdate();

	// 添加事件
	static void AddListener(int key, std::string tag, std::function<void()> fun);
	// 移除事件
	static void RemoveListener(int key, std::string tag);

	static Vector2 GetMousePosition();
	static bool GetMouseButtonDown(int buttonID);
	static bool GetMouseButton(int buttonID);
	static bool GetMouseButtonUp(int buttonID);

	static bool GetKeyDown(int buttonID);
	static bool GetKey(int buttonID);
	static bool GetKeyUp(int buttonID);
};

