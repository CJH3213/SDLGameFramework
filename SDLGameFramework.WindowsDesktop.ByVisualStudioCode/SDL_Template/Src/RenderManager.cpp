#include "RenderManager.h"
#include "WindowsManager.h"
#include "Transform.h"
//#include "MyTime.h"

RenderManager& RenderManager::GetInstance()
{
    static RenderManager instance;
    return instance;
}

void RenderManager::AddRenderer(std::weak_ptr<RenderInterface> wp)
{
    auto& ins = GetInstance();
    auto& renderers = ins.mRenderers;
    auto rif = wp.lock();
    int drawOrder = rif->GetOrder();

    auto it = renderers.rbegin();
    // 寻找绘制层级稍大的第一个对象，插入它前面
    for (; it != renderers.rend(); it++)
    {
        if (it->expired() == true)
            continue;

        if (drawOrder <= it->lock()->GetOrder())
            break;
    }

    auto it2 = it.base(); 
    renderers.insert(it2, wp);
    //注意base返回正向迭代器是反向迭代器的右一位
    // 如rbegin.base == end，这是为了方便insert
}

void RenderManager::AddCamera(ComponentPtr<Camera> comPtr)
{
    GetInstance().mCameras.emplace_back(comPtr);
}

ComponentPtr<Camera> RenderManager::GetMainCamera()
{
    ComponentPtr<Camera> mainCam;

    auto& cams = GetInstance().mCameras;
    for (auto& cam : cams)
        if (cam == false && (*cam).mIsMainCam)
        {
            mainCam = cam;
            break;
        }

    return mainCam;
}

void RenderManager::ChangeOrder(RenderInterface& rif, int order)
{
    auto& ins = GetInstance();
    auto& renderers = ins.mRenderers;
    auto pRif1 = &rif;
    // 先找到渲染接口在容器中的位置
    auto it = std::find_if(renderers.begin(), renderers.end(),
        [&pRif1](std::weak_ptr<RenderInterface>& ren) {return pRif1 == ren.lock().get(); });
    // 移除后再添加，添加函数会重新根据优先级插入适当位置
    if (it != renderers.end())
    {
        std::weak_ptr<RenderInterface> wp = *it;
        renderers.erase(it);
        AddRenderer(wp);
    }
}

void RenderManager::Update()
{
    auto& mainWin = WindowsManager::Get(0);

    auto& ins = GetInstance();
    auto& renderers = ins.mRenderers;
    auto& cameras = ins.mCameras;

    // 剔除失效摄像头
    for (auto it = cameras.begin(); it != cameras.end(); )
    {
        if ((*it) == true)
            it = cameras.erase(it);
        else
            it++;
    }
    // 剔除失效渲染对象（每一帧中最耗时就是这里！！！）
    //auto t1 = MyTime::Time_s();
    for (auto it = renderers.begin(); it != renderers.end(); )
    {
        if ((*it).expired() == true)   // 这里可以提出去，不必在每个摄像机里都判断一遍
            it = renderers.erase(it);
        else
            it++;
    }
    //auto t2 = MyTime::Time_s();
    //auto t3 = t2 - t1;
    //SDL_Log("Render Remove Time : %f", t3);

    // 最小化时不能渲染，会内存暴涨
    if (mainWin.IsMinimized() == true)
        return;

    // 清空渲染器
    //mainWin.SetRenderTargetForWin();
    mainWin.SetPenColor(Color::Blue);
    mainWin.Clean();

    // 将所有摄像机分别设置为渲染对象
    for (auto it = cameras.begin(); it != cameras.end(); it++)
    {
        auto& cam_ptr = (*it);
        if (cam_ptr == false)
        {
            auto& cam_com = *cam_ptr;
            auto& cam_trans = cam_com.GetTransform();
            mainWin.SetRenderTarget(cam_com.mTexture);
            mainWin.SetPenColor(Color::White);
            mainWin.Clean();

            // 执行所有渲染组件的绘制方法
            for (auto it = renderers.begin(); it != renderers.end(); it++)
            {
                auto& wp = (*it);
                if (wp.expired() == false)
                {
                    auto sp = wp.lock();
                    (*sp).Draw(cam_com);
                }
            }
            //SDL_Log("renderers : %d", renderers.size());

            // 摄像机产生的纹理图绘制到窗口
            mainWin.SetRenderTargetForWin();
            mainWin.DrawTexture(cam_com.mTexture, Rectangle::Zero, 
                {cam_com.mWinPos, 
                cam_com.mTexture.mSize},  // 绘制到窗口内的位置和大小
                0.0f);

        }
    }

    // 交换帧缓存
    mainWin.Present();
}
