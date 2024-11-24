#pragma once
#include "Component.h"
#include <vector>
#include "Vector2.h"
#include "Matrix4x4.h"

class Transform :
    public Component
{
protected:
    // ˫�����ṹ��ÿ���任�ڵ�������һ�����ڵ㣬�����ж���ӽڵ�
    Transform* mParentTrans = nullptr;
    std::vector<Transform*> mChildren;

    // �븸����ϳ���ϵ
    void SeverWithParent();

public:
    Vector2 mLocalPosition = Vector2::Zero;
    double mLocalRotation = 0.0f;
    Vector2 mLocalScale = Vector2::One;

    // ��ȡ����λ��
    Vector2 GetPosition() const;
    // ��������λ��
    void SetPosition(Vector2 p);
    // ��ȡ������ת�Ƕ�
    float GetRotation() const;
    // ����������ת�Ƕ�
    void SetRotation(float r);
    // ��ȡ�������ű�
    Vector2 GetScale() const;
    // �����������ű�
    void SetScale(Vector2 v);

    // ���ø���ϵ
    void SetParent(Transform* parent);
    // ��ȡֱ���Ӷ���任���
    auto& GetChildren() { return mChildren; }

    Matrix4x4 LocalToWorldMatrix();

    virtual void OnDestroy() override;
};

