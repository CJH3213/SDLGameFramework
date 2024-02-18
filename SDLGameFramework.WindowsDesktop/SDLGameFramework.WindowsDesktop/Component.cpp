#include "Component.h"
#include "GameObject.h"
#include "Scene.h"

Transform& Component::GetTransform()
{
    return mGameObject->GetTransform();
}

bool Component::operator==(const Component& other) const
{
    // 通过比较两个原始指针是否相同，判断两个组件是否同一对象 ？
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
        DestroyAllChildren(*(child->mGameObject));	//递归销毁
        Destroy(*(child->mGameObject));
    }
}
