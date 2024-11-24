#include "Input.h"
#include <SDL.h>
#include "Debug.h"

Input::Input()
{
	for (int i = 0; i < 3; i++)
	{
		mMouseBottons[i][0] = false;
		mMouseBottons[i][1] = false;
	}

	for (int i = 0; i < SDL_NUM_SCANCODES; i++)
	{
		mKeyboard[i][0] = false;
		mKeyboard[i][1] = false;
	}
}

Input& Input::GetInstance()
{
	static Input instance;	// 通过静态局部变量确保只创建一次实例
	return instance;
}

void Input::FrameUpdate()
{
	auto& ins = GetInstance();

	// 更新鼠标事件
	int x, y;
	auto currMouseButtons = SDL_GetMouseState(&x, &y);
	ins.mMousePos = Vector2(x, y);
	for (int i = 0; i < 3; i++)
	{
		ins.mMouseBottons[i][0] = ins.mMouseBottons[i][1];
		ins.mMouseBottons[i][1] = currMouseButtons & (1<<i);
		// SDL鼠标键代码：1左，2中，4右
	}

	// 更新键盘状态
	auto currKeyboard = SDL_GetKeyboardState(NULL);
	for (int i = 0; i < SDL_NUM_SCANCODES; i++)
	{
		ins.mKeyboard[i][0] = ins.mKeyboard[i][1];
		ins.mKeyboard[i][1] = currKeyboard[i] != 0;
	}
}

void Input::AddListener(int key, std::string tag, std::function<void()> fun)
{
	GetInstance().mEvent.Add(key, tag, fun);
}

void Input::RemoveListener(int key, std::string tag)
{
	GetInstance().mEvent.Remove(key, tag);
}

Vector2 Input::GetMousePosition()
{
	return GetInstance().mMousePos;
}

bool Input::GetMouseButtonDown(int buttonID)
{
	if (buttonID < 0 || buttonID > 3)
		return false;

	auto& mouseBottons = GetInstance().mMouseBottons;
	return !mouseBottons[buttonID][0] && mouseBottons[buttonID][1];
}

bool Input::GetMouseButton(int buttonID)
{
	if (buttonID < 0 || buttonID > 3)
		return false;

	auto& mouseBottons = GetInstance().mMouseBottons;
	return mouseBottons[buttonID][1];
}

bool Input::GetMouseButtonUp(int buttonID)
{
	if (buttonID < 0 || buttonID > 3)
		return false;

	auto& mouseBottons = GetInstance().mMouseBottons;
	return mouseBottons[buttonID][0] && !mouseBottons[buttonID][1];
}

bool Input::GetKeyDown(int buttonID)
{
	if (buttonID < 0 || buttonID > SDL_NUM_SCANCODES)
		return false;

	auto& keyboard = GetInstance().mKeyboard;
	return !keyboard[buttonID][0] && keyboard[buttonID][1];
}

bool Input::GetKey(int buttonID)
{
	if (buttonID < 0 || buttonID > SDL_NUM_SCANCODES)
		return false;

	auto& keyboard = GetInstance().mKeyboard;
	return keyboard[buttonID][1];
}

bool Input::GetKeyUp(int buttonID)
{
	if (buttonID < 0 || buttonID > SDL_NUM_SCANCODES)
		return false;

	auto& keyboard = GetInstance().mKeyboard;
	return keyboard[buttonID][0] && !keyboard[buttonID][1];
}

void Input::SystemUpdate()
{
	auto& ins = GetInstance();
	SDL_Event event;

	// 消息泵，遍历未处理的事件
	while (SDL_PollEvent(&event))
	{
		//SDL_Log("EVENT CODE: %d", event.type);
		if (event.type & (SDL_QUIT | SDL_RENDER_TARGETS_RESET))
			ins.mEvent.Invoke(event.type);
	}
}
