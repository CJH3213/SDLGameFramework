#include "Component.h"
#include "GameObject.h"
#include "Scene.h"

Transform& Component::GetTransform()
{
    return mGameObject->GetTransform();
}

bool Component::operator==(const Component& other) const
{
    // ͨ���Ƚ�����ԭʼָ���Ƿ���ͬ���ж���������Ƿ�ͬһ���� ��
    Component* pThis = mSelfPtr.lock().get();
    Component* pOther = other.mSelfPtr.lock().get();
    return pThis == pOther;
}

void Component::StartCoroutine(CoRet coRet)
{
    mCoRets.emplace_back(coRet);
}

Scene* Component::GetScene()
{
    return mGameObject->mScene;
}

GameObject* Component::GetGameObject()
{
    return mGameObject;
}

void Component::_OnDestroy()
{
    for (auto& coRet : mCoRets)
    {
        coRet._h.promise().mStopFlag = true;
    }
    mCoRets.clear();

    OnDestroy();
}

void Component::Destroy(GameObject& gObj)
{
    GetScene()->RemoveGameObject(gObj);
}

void Component::DestroyAllChildren(GameObject& gObj)
{
    auto& trans = gObj.GetTransform();
    auto& children = trans.GetChildren();
    for (auto& child : children)
    {
        DestroyAllChildren(*(child->mGameObject));	//�ݹ�����
        Destroy(*(child->mGameObject));
    }
}
