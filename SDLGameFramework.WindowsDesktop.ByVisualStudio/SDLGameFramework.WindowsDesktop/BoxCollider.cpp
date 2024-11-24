#include "BoxCollider.h"
#include "Debug.h"
#include "Transform.h"
#include "Input.h"

void BoxCollider::Awake()
{
	mColliderIF = std::make_shared<BoxColliderIF>(this);
	CollisionManager::AddCollider(mColliderIF);
    
    mBounds = { Vector2::Zero, {20, 20} };
}

void BoxCollider::Update()
{
    Color color = mCollisionState.size() ? Color::Red : Color::Black;
    Debug::DrawRectangle(mColliderIF->GetBounds(), color);

    Color color2 = mMouseHover[0] ? Color::White : Color::Black;
    Rectangle bounds2 = mColliderIF->GetBounds();
    bounds2.mSize -= {4, 4};
    Debug::DrawRectangle(bounds2, color2);
}

Rectangle BoxCollider::BoxColliderIF::GetBounds()
{
    auto& trans = mOwner->GetTransform();
    return { trans.GetPosition() + mOwner->mBounds.mCenterPos,
        mOwner->mBounds.mSize * trans.GetScale()};
}

void BoxCollider::BoxColliderIF::OnCollisionCallback(ColliderInterface& colliderIF)
{
    // 注意区分以下关系：
    // ColliderInterface：向 碰撞管理器 注册 碰撞器 的接口
    // BoxColliderIF：用于衔接 碰撞组件 和 碰撞接口 的类
    // BoxCollider：碰撞组件，可以添加至 游戏对象

    // 尝试将碰撞器接口转为组件的碰撞器接口，其他类型无法处理
    auto boxColliderIF = dynamic_cast<BoxColliderIF*>(&colliderIF);
    if(boxColliderIF != nullptr)
    {
        auto boxColliderCom = boxColliderIF->mOwner;
        auto& collisionState = boxColliderCom->mCollisionState;
        auto it = collisionState.find(boxColliderCom);
        if (it == collisionState.end())
            collisionState.emplace(boxColliderCom, std::array<bool, 2>{false, true});
        else
            (*it).second[1] = true;
        
        return;
    }

    // 是不是鼠标指针碰撞器？
    auto mouseColliderIF = dynamic_cast<MouseColliderIF*>(&colliderIF);
    if (mouseColliderIF != nullptr)
    {
        mOwner->mMouseHover[1] = true;
        if (!mOwner->mMouseClick[0] && Input::GetMouseButtonDown(0))
            mOwner->mMouseClick[1] = true;
    }
}

void BoxCollider::BoxColliderIF::LateCollideUpdate()
{
    auto& collisionState = mOwner->mCollisionState;
    // 根据前后两帧的状态调用对应的回调函数
    for (auto it = collisionState.begin(); it != collisionState.end();)
    {
        if ((*it).second[0] == false)
        {
            if ((*it).second[1] == false)  // 0 0 碰撞关系已结束，销毁记录
            {
                it = collisionState.erase(it);
                continue;
            }
            else  // 0 1 初次碰撞
                mOwner->mEnterEvents.Invoke(*(*it).first);
        }
        else
        {
            if ((*it).second[1] == true)    // 1 1 持续碰撞
                mOwner->mStayEvents.Invoke(*(*it).first);
            else    // 1 0 结束碰撞
                mOwner->mExitEvents.Invoke(*(*it).first);
        }

        // 为下次遍历准备状态
        (*it).second[0] = (*it).second[1];
        (*it).second[1] = false;

        it++;
    }

    // ****** //

    auto& mouseHover = mOwner->mMouseHover;
    // 鼠标悬停事件
    if (mouseHover[0] == false)
    {
        if (mouseHover[1] == true) //这一帧鼠标进入
            mOwner->mMouseEnterEvents.Invoke();
    }
    else
    {
        if (mouseHover[1] == true)//鼠标持续悬停
            mOwner->mMouseOverEvents.Invoke();
        else//鼠标离开
            mOwner->mMouseExitEvents.Invoke();
    }
    // 将本次记录值作为老记录，为下次作准备
    mouseHover[0] = mouseHover[1];
    mouseHover[1] = false;

    auto& mouseClick = mOwner->mMouseClick;
    // 鼠标点击事件
    if (mouseClick[0] == false)
    {
        if (mouseClick[1] == true)// 0-1：这一帧鼠标左键按下
            mOwner->mMouseDownEvents.Invoke();
    }
    else
    {
        // 注意：此处鼠标按下不一定还悬停在物体上
        if (Input::GetMouseButton(0))
            mouseClick[1] = true;

        if (mouseClick[1] == true)// 1-1：鼠标左键持续按下
            mOwner->mMouseDragEvents.Invoke();
        else  // 1-0：鼠标左键释放
        {
            mOwner->mMouseUpEvents.Invoke();

            // 鼠标左键松开时，仍然悬停在碰撞器上，调用对应回调
            if (mouseHover[0])
                mOwner->mMouseUpAsButtonEvents.Invoke();
        }
    }
    // 将本次记录值作为老记录，为下次作准备
    mouseClick[0] = mouseClick[1];
    mouseClick[1] = false;
}
