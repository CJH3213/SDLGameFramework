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
	// ��ǰ�ڵ�洢����ײ����������ֵ����ʹ���ӽڵ�
	uint16_t mThreshold = 5;
	// ��ǰ�ڵ�İ�Χ��
	Rectangle mBounds;
	// ���浱ǰ��������ײ�ڵ�
	std::vector<std::weak_ptr<ColliderInterface>> mColliders;
	// �ĸ��ӽڵ�
	std::array<std::unique_ptr<CollisionNode>, 4> mSubNodes;

	CollisionNode(Rectangle bound) :mBounds(bound) {};

	// ��ȡ�ӽڵ��Χ��
	Rectangle GetSubNodeBounds(uint8_t index);
	// ʵ�����ӽڵ�
	void CreateSubNode(uint8_t index);
	// �����ײ������ǰ�ڵ���ӽڵ㣨�������ڵ㹦�ܣ�
	bool AddCollider(std::weak_ptr<ColliderInterface> collider);
	// �����ײ�����ӽڵ㣨��������ǰ�ڵ㣩
	//bool AddColliderInSub(std::weak_ptr<ColliderInterface> collider);
	// ����ӵ�ǰ�ڵ㿪ʼ���ӽڵ����ײ������
	size_t GetCollidersCount();
	// ά����ǰ�ڵ㼰�ӽڵ㴢�����ײ��
	void MaintainTree();
	// ָ����ײ���뵱ǰ�ڵ���ӽڵ���ײ���
	void DetectionWith(ColliderInterface& collider_wp);
	// �Ե�ǰ�ڵ��Լ��ӽڵ�������ײ����ײ���
	void DetectionAll();
	// *** �����ã����ư�Χ�У��ǳ����٣�
	void DrawBounds();
};

class CollisionManager
{
private:
	// ����ģʽ�£����ù���������ֵ
	CollisionManager() {};
	~CollisionManager() = default;
	CollisionManager(const CollisionManager&) = delete;
	CollisionManager& operator=(const CollisionManager&) = delete;
	// ��ȡ�����൥��
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

