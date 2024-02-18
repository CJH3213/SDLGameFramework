#pragma once
#include "Component.h"
#include "Texture.h"
#include <vector>
//#include "RenderInterface.h"
#include "Window.h"

class Camera :
    public Component
{
private:

public:

    bool mIsMainCam = false;
    bool mIsFullWin = true;  // �Ƿ�ȫ������ʾ
    Texture mTexture;
    Vector2 mWinPos;     // ����������Դ������Ͻ�����

    ComponentPtr<Camera> GetComponentPtr();
    void ResetSize(Vector2 s);

    virtual void Awake() override;
    virtual void LateUpdate() override;

    // �������꣨������꣩ת ��������
    Vector2 WinToWorldPoint(const Vector2& winPos);
    // �������� ת ��������
    Vector2 WorldToWinPoint(const Vector2& worldPos);
};

