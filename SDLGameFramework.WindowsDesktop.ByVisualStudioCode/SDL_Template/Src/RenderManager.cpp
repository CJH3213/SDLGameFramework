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
    // Ѱ�һ��Ʋ㼶�Դ�ĵ�һ�����󣬲�����ǰ��
    for (; it != renderers.rend(); it++)
    {
        if (it->expired() == true)
            continue;

        if (drawOrder <= it->lock()->GetOrder())
            break;
    }

    auto it2 = it.base(); 
    renderers.insert(it2, wp);
    //ע��base��������������Ƿ������������һλ
    // ��rbegin.base == end������Ϊ�˷���insert
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
    // ���ҵ���Ⱦ�ӿ��������е�λ��
    auto it = std::find_if(renderers.begin(), renderers.end(),
        [&pRif1](std::weak_ptr<RenderInterface>& ren) {return pRif1 == ren.lock().get(); });
    // �Ƴ�������ӣ���Ӻ��������¸������ȼ������ʵ�λ��
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

    // �޳�ʧЧ����ͷ
    for (auto it = cameras.begin(); it != cameras.end(); )
    {
        if ((*it) == true)
            it = cameras.erase(it);
        else
            it++;
    }
    // �޳�ʧЧ��Ⱦ����ÿһ֡�����ʱ�������������
    //auto t1 = MyTime::Time_s();
    for (auto it = renderers.begin(); it != renderers.end(); )
    {
        if ((*it).expired() == true)   // ����������ȥ��������ÿ��������ﶼ�ж�һ��
            it = renderers.erase(it);
        else
            it++;
    }
    //auto t2 = MyTime::Time_s();
    //auto t3 = t2 - t1;
    //SDL_Log("Render Remove Time : %f", t3);

    // ��С��ʱ������Ⱦ�����ڴ汩��
    if (mainWin.IsMinimized() == true)
        return;

    // �����Ⱦ��
    //mainWin.SetRenderTargetForWin();
    mainWin.SetPenColor(Color::Blue);
    mainWin.Clean();

    // ������������ֱ�����Ϊ��Ⱦ����
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

            // ִ��������Ⱦ����Ļ��Ʒ���
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

            // ���������������ͼ���Ƶ�����
            mainWin.SetRenderTargetForWin();
            mainWin.DrawTexture(cam_com.mTexture, Rectangle::Zero, 
                {cam_com.mWinPos, 
                cam_com.mTexture.mSize},  // ���Ƶ������ڵ�λ�úʹ�С
                0.0f);

        }
    }

    // ����֡����
    mainWin.Present();
}
