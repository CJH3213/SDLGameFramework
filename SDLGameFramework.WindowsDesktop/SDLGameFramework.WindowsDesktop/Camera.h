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
    bool mIsFullWin = true;  // 是否全窗口显示
    Texture mTexture;
    Vector2 mWinPos;     // 纹理中心相对窗口左上角坐标

    ComponentPtr<Camera> GetComponentPtr();
    void ResetSize(Vector2 s);

    virtual void Awake() override;
    virtual void LateUpdate() override;

    // 窗口坐标（鼠标坐标）转 世界坐标
    Vector2 WinToWorldPoint(const Vector2& winPos);
    // 世界坐标 转 窗口坐标
    Vector2 WorldToWinPoint(const Vector2& worldPos);
};

