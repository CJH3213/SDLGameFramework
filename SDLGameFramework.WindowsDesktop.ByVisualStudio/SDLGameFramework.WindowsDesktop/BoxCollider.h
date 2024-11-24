#pragma once
#include "Component.h"
#include "CollisionManager.h"
#include <memory>

class BoxCollider :
    public Component
{
private:
	struct BoxColliderIF : ColliderInterface
	{
	private:
		BoxCollider* mOwner;
	public:
		BoxColliderIF(BoxCollider* owner) :mOwner(owner) {}
		class BoxCollider* GetBoxCollider() { return mOwner; }
		virtual Rectangle GetBounds() override;
		virtual void OnCollisionCallback(ColliderInterface& collider) override;
		virtual void LateCollideUpdate() override;
	};
	std::shared_ptr<BoxColliderIF> mColliderIF;

public:
	// ��ײ���İ�Χ��
	Rectangle mBounds;
	// ��¼��֮��ײ�������״̬
	std::map<BoxCollider*, std::array<bool, 2>> mCollisionState;
	// ��¼�����ͣ״̬
	bool mMouseHover[2] = { false, false };
	// ��¼�����״̬
	bool mMouseClick[2] = { false, false };

	// ��ײ�����¼��ص�
	Event<void(BoxCollider&)> mEnterEvents;
	// ��ײ�����¼��ص�
	Event<void(BoxCollider&)> mStayEvents;
	// ��ײ�����¼��ص�
	Event<void(BoxCollider&)> mExitEvents;

	// �����������ײ���Ϸ��ĵ�һ֡���ǵ����
	Event<void()> mMouseEnterEvents;
	// ��������������ײ����ÿһ֡���ǵ����
	Event<void()> mMouseOverEvents;
	// ��������Ƴ���ײ��ʱ���һ֡���ǵ����
	Event<void()> mMouseExitEvents;

	// �������ײ���ĵ�һ֡�������
	Event<void()> mMouseDownEvents;
	// �������ײ����ÿһ֡�������
	Event<void()> mMouseDragEvents;
	// �������ײ�����ͷ�һ֡����������ͷŲ���֤������ײ����
	Event<void()> mMouseUpEvents;
	// �������ײ�����ͷ�һ֡���������ֻ�е�����ͷ�ͬһ����ײ���ŵ���
	Event<void()> mMouseUpAsButtonEvents;
	Event<void()>& OnClick = mMouseUpAsButtonEvents;

	virtual void Awake() override;
	virtual void Update() override;

	//Rectangle GetWorldBounds() { return mColliderIF->GetBounds(); }
};

// BoxColliderĿǰ����ƽ�о�����ײ������֧����ת��֧��λ�ƺ�����
