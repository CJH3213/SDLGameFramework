#pragma once
#include "Component.h"
#include <vector>
#include "Vector2.h"
#include "Matrix4x4.h"

class Transform :
    public Component
{
protected:
    // 双向树结构，每个变换节点向上有一个父节点，向下有多个子节点
    Transform* mParentTrans = nullptr;
    std::vector<Transform*> mChildren;

    // 与父对象断除关系
    void SeverWithParent();

public:
    Vector2 mLocalPosition = Vector2::Zero;
    double mLocalRotation = 0.0f;
    Vector2 mLocalScale = Vector2::One;

    // 获取世界位置
    Vector2 GetPosition() const;
    // 设置世界位置
    void SetPosition(Vector2 p);
    // 获取世界旋转角度
    float GetRotation() const;
    // 设置世界旋转角度
    void SetRotation(float r);
    // 获取世界缩放比
    Vector2 GetScale() const;
    // 设置世界缩放比
    void SetScale(Vector2 v);

    // 设置父关系
    void SetParent(Transform* parent);
    // 获取直属子对象变换组件
    auto& GetChildren() { return mChildren; }

    Matrix4x4 LocalToWorldMatrix();

    virtual void OnDestroy() override;
};

