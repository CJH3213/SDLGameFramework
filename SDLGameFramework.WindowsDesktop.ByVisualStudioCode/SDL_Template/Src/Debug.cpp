#include "Debug.h"
#include "WindowsManager.h"
#include "RenderManager.h"

CoRet Debug::CoFun_WaitSec(std::shared_ptr<LineRenIF> sp, float sec)
{
    Yield WaitForSeconds(sec);
    co_return 0;
}

Debug& Debug::GetInstance()
{
    static Debug instance;
    return instance;
}

void Debug::DrawLine(Vector2 pos1, Vector2 pos2, Color color, float sec)
{
    auto mRenderIF = std::make_shared<LineRenIF>(pos1, pos2, color);
    RenderManager::AddRenderer(mRenderIF);

    // 利用协程对象会保存形参的特性，维持渲染接口一段时间后再销毁
    CoFun_WaitSec(mRenderIF, sec);
}

void Debug::DrawRectangle(Rectangle rect, Color color, float sec)
{
    Vector2 corners[4] = {
        rect.GetRightTopPos(),
        rect.GetLeftTopPos(),
        rect.GetLeftBottomPos(),
        rect.GetRightBottomPos()
    };
    Vector2 lines[4][2] = {
        {corners[0], corners[1]},
        {corners[1], corners[2]},
        {corners[2], corners[3]},
        {corners[3], corners[0]}
    };

    for(int i=0; i<4; i++)
        DrawLine(lines[i][0], lines[i][1], color, sec);
}

void Debug::LineRenIF::Draw(Camera& cam)
{
    auto& win = WindowsManager::Get(0);
    win.SetPenColor(mColor);
    win.DrawLine(cam.WorldToWinPoint(mPos1), cam.WorldToWinPoint(mPos2));
}
