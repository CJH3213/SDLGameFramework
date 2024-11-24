#pragma once
#include "Event.h"
#include "Vector2.h"

class Input
{
private:
	// ����ģʽ�£����ù���������ֵ
	Input();
	~Input() = default;
	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;

	// �¼�����
	Event<int, void()> mEvent;
	
	// ��갴ť״̬
	bool mMouseBottons[3][2];
	// �������
	Vector2 mMousePos;
	// ���̰�ť״̬
	bool mKeyboard[SDL_NUM_SCANCODES][2];

public:
	// ��ȡ�����൥��
	static Input& GetInstance();

	/* ����������ʵĺ�����Ϊstatic���ͣ���ӷ��ʵ��� */

	// �������¼��ġ�ϵͳ������
	static void SystemUpdate();
	// ÿ֡����
	static void FrameUpdate();

	// ����¼�
	static void AddListener(int key, std::string tag, std::function<void()> fun);
	// �Ƴ��¼�
	static void RemoveListener(int key, std::string tag);

	static Vector2 GetMousePosition();
	static bool GetMouseButtonDown(int buttonID);
	static bool GetMouseButton(int buttonID);
	static bool GetMouseButtonUp(int buttonID);

	static bool GetKeyDown(int buttonID);
	static bool GetKey(int buttonID);
	static bool GetKeyUp(int buttonID);
};

