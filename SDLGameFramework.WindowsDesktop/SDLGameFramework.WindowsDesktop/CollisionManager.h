#pragma once
#include <vector>
#include <array>
#include <memory>
#include "Rectangle.h"
#include <map>
#include "Event.h"

struct ColliderInterface
{
	virtual Rectangle GetBounds();
	virtual void OnCollisionCallback(ColliderInterface& collider) {};
	virtual void LateCollideUpdate() {};
};

struct MouseColliderIF : ColliderInterface
{
	Vector2 mMousePos;
	virtual Rectangle GetBounds() override;
};

struct CollisionNode
{
	// 当前节点存储的碰撞器超过该阈值后，再使用子节点
	uint16_t mThreshold = 5;
	// 当前节点的包围盒
	Rectangle mBounds;
	// 储存当前容器的碰撞节点
	std::vector<std::weak_ptr<ColliderInterface>> mColliders;
	// 四个子节点
	std::array<std::unique_ptr<CollisionNode>, 4> mSubNodes;

	CollisionNode(Rectangle bound) :mBounds(bound) {};

	// 获取子节点包围盒
	Rectangle GetSubNodeBounds(uint8_t index);
	// 实例化子节点
	void CreateSubNode(uint8_t index);
	// 添加碰撞器到当前节点或子节点（无外扩节点功能）
	bool AddCollider(std::weak_ptr<ColliderInterface> collider);
	// 添加碰撞器到子节点（不包括当前节点）
	//bool AddColliderInSub(std::weak_ptr<ColliderInterface> collider);
	// 计算从当前节点开始至子节点的碰撞器总数
	size_t GetCollidersCount();
	// 维护当前节点及子节点储存的碰撞器
	void MaintainTree();
	// 指定碰撞器与当前节点和子节点碰撞检测
	void DetectionWith(ColliderInterface& collider_wp);
	// 对当前节点以及子节点所有碰撞器碰撞检测
	void DetectionAll();
	// *** 调试用：绘制包围盒（非常卡顿）
	void DrawBounds();
};

class CollisionManager
{
private:
	// 单例模式下，禁用构造析构赋值
	CollisionManager() {};
	~CollisionManager() = default;
	CollisionManager(const CollisionManager&) = delete;
	CollisionManager& operator=(const CollisionManager&) = delete;
	// 获取输入类单例
	static CollisionManager& GetInstance();

private:
	std::unique_ptr<CollisionNode> mRootNode;
	std::shared_ptr<MouseColliderIF> mMouseCollider;

public:
	static void AddCollider(std::weak_ptr<ColliderInterface> collider);

	static void Init();
	static void Close() {};
	static void Update();
};

