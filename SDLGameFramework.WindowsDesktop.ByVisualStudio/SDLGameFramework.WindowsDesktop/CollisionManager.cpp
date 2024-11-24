#include "CollisionManager.h"
#include "Debug.h"
#include "RenderManager.h"
#include "Input.h"

CollisionManager& CollisionManager::GetInstance()
{
    static CollisionManager instance;
    return instance;
}

void CollisionManager::AddCollider(std::weak_ptr<ColliderInterface> collider_wp)
{
    auto& ins = GetInstance();
    auto collider = collider_wp.lock();

    // 如果根节点容纳不下，向外拓展节点，直到能容纳为止
    while(collider->GetBounds().IsInside(ins.mRootNode->mBounds) == false)
    {
        // 寻找碰撞器离根节点的哪一个角最近
        std::array< Vector2, 4> corners = {
            ins.mRootNode->mBounds.GetRightTopPos(),
            ins.mRootNode->mBounds.GetLeftTopPos(),
            ins.mRootNode->mBounds.GetLeftBottomPos(),
            ins.mRootNode->mBounds.GetRightBottomPos()
        };

        float minDistance = (corners[0] - collider->GetBounds().mCenterPos).LengthUNSquared();
        uint8_t minIndex = 0;
        for (uint8_t i = 1; i<corners.size(); i ++)
        {
            float distance = (corners[i] - collider->GetBounds().mCenterPos).LengthUNSquared();
            if (distance < minDistance)
            {
                minDistance = distance;
                minIndex = i;
            }
        }

        // 以该角点为中心创建更大的节点
        Rectangle newBound = { corners.at(minIndex), ins.mRootNode->mBounds.mSize * 2 };
        auto newNode = std::make_unique<CollisionNode>(newBound);

        // 新的大节点作为根节点，原根节点储存在新节点里
        uint8_t diagonalIndex = (minIndex + 2) % 4;
        newNode->mSubNodes.at(diagonalIndex) = std::move(ins.mRootNode);
        ins.mRootNode = std::move(newNode);
    }

    // 将碰撞器从根节点添加进去（实际可能会添加到子节点上）
    ins.mRootNode->AddCollider(collider);

}

void CollisionManager::Init()
{
    auto& ins = GetInstance();

    Rectangle bound = { Vector2::Zero, {100, 100} };
    ins.mRootNode = std::make_unique<CollisionNode>(bound);

    ins.mMouseCollider = std::make_shared<MouseColliderIF>();
    //ins.mMouseCollider->GetBounds() = { Vector2::Zero, {5, 5} };
    CollisionManager::AddCollider(ins.mMouseCollider);
}

void CollisionManager::Update()
{
    auto& ins = GetInstance();

    // 更新鼠标位置
    auto mainCam = RenderManager::GetMainCamera();
    Vector2 clickPos = Input::GetMousePosition();
    Vector2 worldPos = (*mainCam).WinToWorldPoint(clickPos);
    ins.mMouseCollider->mMousePos = worldPos;
    Debug::DrawRectangle(ins.mMouseCollider->GetBounds(), Color::White);

    // 维护四叉树
    ins.mRootNode->MaintainTree();

    // 物与物碰撞检测
    ins.mRootNode->DetectionAll();

    // *** 调试用：显示所有节点包围盒
    size_t sss = ins.mRootNode->GetCollidersCount();
    //Debug::Log(u8"碰撞器个数: %d", sss);
    ins.mRootNode->DrawBounds();
}




Rectangle CollisionNode::GetSubNodeBounds(uint8_t index)
{
    // 若已经存在子节点，直接读取子节点的包围盒
    if (mSubNodes[index] != nullptr)
        return mSubNodes[index]->mBounds;

    // 若该子节点还未存在，计算包围盒
    Rectangle tempRect = mBounds;
    tempRect.mSize *= 0.5f;
    switch (index)
    {
    case 0:
        return { tempRect.GetRightTopPos(), tempRect.mSize };
    case 1:
        return{ tempRect.GetLeftTopPos(), tempRect.mSize };
    case 2:
        return{ tempRect.GetLeftBottomPos(), tempRect.mSize };
    case 3:
        return{ tempRect.GetRightBottomPos(), tempRect.mSize };
    default:
        return Rectangle();
    }
}

void CollisionNode::CreateSubNode(uint8_t index)
{
    mSubNodes[index] = std::make_unique<CollisionNode>(GetSubNodeBounds(index));
    //将当前节点容器对象尝试添加到子节点
    for (auto it = mColliders.begin(); it != mColliders.end();)
    {
        if (mSubNodes[index]->AddCollider(*it))	//添加成功的就删除
            it = mColliders.erase(it);
        else it++;
    }
}

bool CollisionNode::AddCollider(std::weak_ptr<ColliderInterface> collider_wp)
{
    auto collider = collider_wp.lock();

    // 如果碰撞器在当前节点外部，拒绝添加
    if (collider->GetBounds().IsInside(mBounds) == false)
        return false;

    // 从当前节点开始向子节点查找，找到可以容纳该碰撞器的节点
    CollisionNode* node = this;
    while (true)
    {
        // 如果碰撞器在当前节点内部，而且当前节点存储较少时，不存在子节点
        if (node->GetCollidersCount() < node->mThreshold)
            break;

        // 判断能不能被哪个子节点包含
        int i = 0;
        for (; i < 4; i++)
        {
            if (collider->GetBounds().IsInside(node->GetSubNodeBounds(i)))
            {
                if (node->mSubNodes[i] == nullptr)
                    node->CreateSubNode(i);
                node = node->mSubNodes[i].get();
                break;
            }
        }
        if (i >= 4)
            break;
    }

    // 将碰撞器添加到该节点
    node->mColliders.emplace_back(collider);

    return true;
}

//bool CollisionNode::AddColliderInSub(std::weak_ptr<ColliderInterface> collider_wp)
//{
//    auto collider = collider_wp.lock();
//
//    // 从当前节点开始向子节点查找，找到可以容纳该碰撞器的节点
//    CollisionNode* node = this;
//    while (true)
//    {
//        // 如果碰撞器在当前节点内部，而且当前节点存储较少时，不存在子节点
//        if (node->GetCollidersCount() < node->mThreshold)
//            break;
//
//        // 判断能不能被哪个子节点包含
//        int i = 0;
//        for (; i < mSubNodes.size(); i++)
//        {
//            if (collider->GetBounds().IsInside(GetSubNodeBounds(i)))
//            {
//                if (node->mSubNodes[i] == nullptr)
//                    CreateSubNode(i);   // 创建子节点，当前节点部分碰撞器会添加到子节点
//                node = node->mSubNodes[i].get();
//                break;
//            }
//        }
//        if (i >= mSubNodes.size())
//            break;
//    }
//
//    // 将碰撞器添加到该节点
//    if(node != this)
//    {
//        node->mColliders.emplace_back(collider);
//        return true;
//    }
//
//    return false;
//}

size_t CollisionNode::GetCollidersCount()
{
    size_t sum = mColliders.size();

    for (auto& subNode : mSubNodes)
        if (subNode != nullptr)
            sum += subNode->GetCollidersCount();

    return sum;
}

void CollisionNode::MaintainTree()
{
    // 当前节点需要插入子节点的碰撞器，不能在以下遍历过程[*]中插入
    // 若碰撞器需要插入的子节点未实例化，在实例化子节点后
    // 子节点会将当前节点所有碰撞器尝试添加进其中，当前节点会移除一些碰撞器
    // 这会使以下遍历迭代器失效，所以先将要添加进子节点的碰撞器添加到tempCollider
    // 待以下遍历结束后，再将tempCollider中的碰撞器添加到子节点
    std::vector<std::weak_ptr<ColliderInterface>> tempCollider;

    // [*]以下遍历
    for (auto it = mColliders.begin(); it != mColliders.end();)
    {
        // 剔除已经失效的碰撞器
        if (it->expired() == true)
        {
            it = mColliders.erase(it);
            continue;
        }

        auto collider = it->lock();
        // 碰撞器在当前节点外，重新从根节点插入
        if (collider->GetBounds().IsInside(mBounds) == false)
        {
            it = mColliders.erase(it);
            CollisionManager::AddCollider(collider);
            continue;
        }

        // 当前节点允许使用子节点，尝试将碰撞器添加到子节点
        if (GetCollidersCount() >= mThreshold)
        {

            // 在当前节点内，再判断是否可以进一步插入四个子节点
            bool moved = false;
            for (int i = 0; i < mSubNodes.size(); i++)
            {
                if (collider->GetBounds().IsInside(GetSubNodeBounds(i)))
                {
                    tempCollider.emplace_back(*it);
                    it = mColliders.erase(it);
                    moved = true;
                }
            }
            if (moved == true)
                continue;

        }

        // 当前检查的碰撞器没被移动，才能执行到这里
        it++;
    }

    // 对四个子节点进行维护（递归）
    for (auto& subNode : mSubNodes)
        if (subNode != nullptr)
        {
            subNode->MaintainTree();
            if (subNode->GetCollidersCount() == 0)
                subNode.reset(nullptr);
        }

    // 重新插入的碰撞器到子节点
    for (auto& co : tempCollider)
    {
        AddCollider(co);
    }
}

void CollisionNode::DetectionWith(ColliderInterface& other)
{
    if (other.GetBounds().IsIntersect(mBounds) == false)
        return;

    // 与当前节点存储的碰撞器检测
    for (auto& co_wp : mColliders)
    {
        auto co = co_wp.lock();
        if (co.get() != &other &&   // 不能对自己碰撞检测
            other.GetBounds().IsIntersect(co->GetBounds()))
        {
            other.OnCollisionCallback(*co);
            co->OnCollisionCallback(other);
        }
    }

    // 与子节点检测（递归）
    for (auto& subNode : mSubNodes)
    {
        if (subNode != nullptr)
            subNode->DetectionWith(other);
    }
}

void CollisionNode::DetectionAll()
{
    for (auto& co_wp : mColliders)
    {
        auto co = co_wp.lock();
        DetectionWith(*co);
        co->LateCollideUpdate();
    }

    for (auto& subNode : mSubNodes)
    {
        if (subNode != nullptr)
            subNode->DetectionAll();
    }
}

void CollisionNode::DrawBounds()
{
    Debug::DrawRectangle(mBounds, Color::Orange);

    //for (auto& collider_wp : mColliders)
    //{
    //    auto collider = collider_wp.lock();
    //    Color color = collider->mCollisionState.size() ? Color::Red : Color::Black;
    //    Debug::DrawRectangle(collider->mBounds, color);
    //}

    for (auto& subNode : mSubNodes)
    {
        if(subNode != nullptr)
            subNode->DrawBounds();
    }
}

Rectangle ColliderInterface::GetBounds()
{
    return Rectangle();
}

Rectangle MouseColliderIF::GetBounds()
{
    return { mMousePos, {5, 5} };
}
