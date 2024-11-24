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
    // ע���������¹�ϵ��
    // ColliderInterface���� ��ײ������ ע�� ��ײ�� �Ľӿ�
    // BoxColliderIF�������ν� ��ײ��� �� ��ײ�ӿ� ����
    // BoxCollider����ײ�������������� ��Ϸ����

    // ���Խ���ײ���ӿ�תΪ�������ײ���ӿڣ����������޷�����
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

    // �ǲ������ָ����ײ����
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
    // ����ǰ����֡��״̬���ö�Ӧ�Ļص�����
    for (auto it = collisionState.begin(); it != collisionState.end();)
    {
        if ((*it).second[0] == false)
        {
            if ((*it).second[1] == false)  // 0 0 ��ײ��ϵ�ѽ��������ټ�¼
            {
                it = collisionState.erase(it);
                continue;
            }
            else  // 0 1 ������ײ
                mOwner->mEnterEvents.Invoke(*(*it).first);
        }
        else
        {
            if ((*it).second[1] == true)    // 1 1 ������ײ
                mOwner->mStayEvents.Invoke(*(*it).first);
            else    // 1 0 ������ײ
                mOwner->mExitEvents.Invoke(*(*it).first);
        }

        // Ϊ�´α���׼��״̬
        (*it).second[0] = (*it).second[1];
        (*it).second[1] = false;

        it++;
    }

    // ****** //

    auto& mouseHover = mOwner->mMouseHover;
    // �����ͣ�¼�
    if (mouseHover[0] == false)
    {
        if (mouseHover[1] == true) //��һ֡������
            mOwner->mMouseEnterEvents.Invoke();
    }
    else
    {
        if (mouseHover[1] == true)//��������ͣ
            mOwner->mMouseOverEvents.Invoke();
        else//����뿪
            mOwner->mMouseExitEvents.Invoke();
    }
    // �����μ�¼ֵ��Ϊ�ϼ�¼��Ϊ�´���׼��
    mouseHover[0] = mouseHover[1];
    mouseHover[1] = false;

    auto& mouseClick = mOwner->mMouseClick;
    // ������¼�
    if (mouseClick[0] == false)
    {
        if (mouseClick[1] == true)// 0-1����һ֡����������
            mOwner->mMouseDownEvents.Invoke();
    }
    else
    {
        // ע�⣺�˴���갴�²�һ������ͣ��������
        if (Input::GetMouseButton(0))
            mouseClick[1] = true;

        if (mouseClick[1] == true)// 1-1����������������
            mOwner->mMouseDragEvents.Invoke();
        else  // 1-0���������ͷ�
        {
            mOwner->mMouseUpEvents.Invoke();

            // �������ɿ�ʱ����Ȼ��ͣ����ײ���ϣ����ö�Ӧ�ص�
            if (mouseHover[0])
                mOwner->mMouseUpAsButtonEvents.Invoke();
        }
    }
    // �����μ�¼ֵ��Ϊ�ϼ�¼��Ϊ�´���׼��
    mouseClick[0] = mouseClick[1];
    mouseClick[1] = false;
}
