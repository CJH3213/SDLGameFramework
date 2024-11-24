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
	// 碰撞器的包围盒
	Rectangle mBounds;
	// 记录与之碰撞过的组件状态
	std::map<BoxCollider*, std::array<bool, 2>> mCollisionState;
	// 记录鼠标悬停状态
	bool mMouseHover[2] = { false, false };
	// 记录鼠标点击状态
	bool mMouseClick[2] = { false, false };

	// 碰撞进入事件回调
	Event<void(BoxCollider&)> mEnterEvents;
	// 碰撞保持事件回调
	Event<void(BoxCollider&)> mStayEvents;
	// 碰撞分离事件回调
	Event<void(BoxCollider&)> mExitEvents;

	// 鼠标悬浮在碰撞器上方的第一帧（非点击）
	Event<void()> mMouseEnterEvents;
	// 鼠标持续悬浮在碰撞器上每一帧（非点击）
	Event<void()> mMouseOverEvents;
	// 鼠标悬浮移出碰撞器时最后一帧（非点击）
	Event<void()> mMouseExitEvents;

	// 鼠标点击碰撞器的第一帧（点击）
	Event<void()> mMouseDownEvents;
	// 鼠标点击碰撞器的每一帧（点击）
	Event<void()> mMouseDragEvents;
	// 鼠标点击碰撞器后释放一帧（点击），释放不保证仍在碰撞器上
	Event<void()> mMouseUpEvents;
	// 鼠标点击碰撞器后释放一帧（点击），只有点击和释放同一个碰撞器才调用
	Event<void()> mMouseUpAsButtonEvents;
	Event<void()>& OnClick = mMouseUpAsButtonEvents;

	virtual void Awake() override;
	virtual void Update() override;

	//Rectangle GetWorldBounds() { return mColliderIF->GetBounds(); }
};

// BoxCollider目前是轴平行矩形碰撞器，不支持旋转，支持位移和缩放
