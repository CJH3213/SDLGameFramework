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

    // ������ڵ����ɲ��£�������չ�ڵ㣬ֱ��������Ϊֹ
    while(collider->GetBounds().IsInside(ins.mRootNode->mBounds) == false)
    {
        // Ѱ����ײ������ڵ����һ�������
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

        // �Ըýǵ�Ϊ���Ĵ�������Ľڵ�
        Rectangle newBound = { corners.at(minIndex), ins.mRootNode->mBounds.mSize * 2 };
        auto newNode = std::make_unique<CollisionNode>(newBound);

        // �µĴ�ڵ���Ϊ���ڵ㣬ԭ���ڵ㴢�����½ڵ���
        uint8_t diagonalIndex = (minIndex + 2) % 4;
        newNode->mSubNodes.at(diagonalIndex) = std::move(ins.mRootNode);
        ins.mRootNode = std::move(newNode);
    }

    // ����ײ���Ӹ��ڵ���ӽ�ȥ��ʵ�ʿ��ܻ���ӵ��ӽڵ��ϣ�
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

    // �������λ��
    auto mainCam = RenderManager::GetMainCamera();
    Vector2 clickPos = Input::GetMousePosition();
    Vector2 worldPos = (*mainCam).WinToWorldPoint(clickPos);
    ins.mMouseCollider->mMousePos = worldPos;
    Debug::DrawRectangle(ins.mMouseCollider->GetBounds(), Color::White);

    // ά���Ĳ���
    ins.mRootNode->MaintainTree();

    // ��������ײ���
    ins.mRootNode->DetectionAll();

    // *** �����ã���ʾ���нڵ��Χ��
    size_t sss = ins.mRootNode->GetCollidersCount();
    //Debug::Log(u8"��ײ������: %d", sss);
    ins.mRootNode->DrawBounds();
}




Rectangle CollisionNode::GetSubNodeBounds(uint8_t index)
{
    // ���Ѿ������ӽڵ㣬ֱ�Ӷ�ȡ�ӽڵ�İ�Χ��
    if (mSubNodes[index] != nullptr)
        return mSubNodes[index]->mBounds;

    // �����ӽڵ㻹δ���ڣ������Χ��
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
    //����ǰ�ڵ�������������ӵ��ӽڵ�
    for (auto it = mColliders.begin(); it != mColliders.end();)
    {
        if (mSubNodes[index]->AddCollider(*it))	//��ӳɹ��ľ�ɾ��
            it = mColliders.erase(it);
        else it++;
    }
}

bool CollisionNode::AddCollider(std::weak_ptr<ColliderInterface> collider_wp)
{
    auto collider = collider_wp.lock();

    // �����ײ���ڵ�ǰ�ڵ��ⲿ���ܾ����
    if (collider->GetBounds().IsInside(mBounds) == false)
        return false;

    // �ӵ�ǰ�ڵ㿪ʼ���ӽڵ���ң��ҵ��������ɸ���ײ���Ľڵ�
    CollisionNode* node = this;
    while (true)
    {
        // �����ײ���ڵ�ǰ�ڵ��ڲ������ҵ�ǰ�ڵ�洢����ʱ���������ӽڵ�
        if (node->GetCollidersCount() < node->mThreshold)
            break;

        // �ж��ܲ��ܱ��ĸ��ӽڵ����
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

    // ����ײ����ӵ��ýڵ�
    node->mColliders.emplace_back(collider);

    return true;
}

//bool CollisionNode::AddColliderInSub(std::weak_ptr<ColliderInterface> collider_wp)
//{
//    auto collider = collider_wp.lock();
//
//    // �ӵ�ǰ�ڵ㿪ʼ���ӽڵ���ң��ҵ��������ɸ���ײ���Ľڵ�
//    CollisionNode* node = this;
//    while (true)
//    {
//        // �����ײ���ڵ�ǰ�ڵ��ڲ������ҵ�ǰ�ڵ�洢����ʱ���������ӽڵ�
//        if (node->GetCollidersCount() < node->mThreshold)
//            break;
//
//        // �ж��ܲ��ܱ��ĸ��ӽڵ����
//        int i = 0;
//        for (; i < mSubNodes.size(); i++)
//        {
//            if (collider->GetBounds().IsInside(GetSubNodeBounds(i)))
//            {
//                if (node->mSubNodes[i] == nullptr)
//                    CreateSubNode(i);   // �����ӽڵ㣬��ǰ�ڵ㲿����ײ������ӵ��ӽڵ�
//                node = node->mSubNodes[i].get();
//                break;
//            }
//        }
//        if (i >= mSubNodes.size())
//            break;
//    }
//
//    // ����ײ����ӵ��ýڵ�
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
    // ��ǰ�ڵ���Ҫ�����ӽڵ����ײ�������������±�������[*]�в���
    // ����ײ����Ҫ������ӽڵ�δʵ��������ʵ�����ӽڵ��
    // �ӽڵ�Ὣ��ǰ�ڵ�������ײ��������ӽ����У���ǰ�ڵ���Ƴ�һЩ��ײ��
    // ���ʹ���±���������ʧЧ�������Ƚ�Ҫ��ӽ��ӽڵ����ײ����ӵ�tempCollider
    // �����±����������ٽ�tempCollider�е���ײ����ӵ��ӽڵ�
    std::vector<std::weak_ptr<ColliderInterface>> tempCollider;

    // [*]���±���
    for (auto it = mColliders.begin(); it != mColliders.end();)
    {
        // �޳��Ѿ�ʧЧ����ײ��
        if (it->expired() == true)
        {
            it = mColliders.erase(it);
            continue;
        }

        auto collider = it->lock();
        // ��ײ���ڵ�ǰ�ڵ��⣬���´Ӹ��ڵ����
        if (collider->GetBounds().IsInside(mBounds) == false)
        {
            it = mColliders.erase(it);
            CollisionManager::AddCollider(collider);
            continue;
        }

        // ��ǰ�ڵ�����ʹ���ӽڵ㣬���Խ���ײ����ӵ��ӽڵ�
        if (GetCollidersCount() >= mThreshold)
        {

            // �ڵ�ǰ�ڵ��ڣ����ж��Ƿ���Խ�һ�������ĸ��ӽڵ�
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

        // ��ǰ������ײ��û���ƶ�������ִ�е�����
        it++;
    }

    // ���ĸ��ӽڵ����ά�����ݹ飩
    for (auto& subNode : mSubNodes)
        if (subNode != nullptr)
        {
            subNode->MaintainTree();
            if (subNode->GetCollidersCount() == 0)
                subNode.reset(nullptr);
        }

    // ���²������ײ�����ӽڵ�
    for (auto& co : tempCollider)
    {
        AddCollider(co);
    }
}

void CollisionNode::DetectionWith(ColliderInterface& other)
{
    if (other.GetBounds().IsIntersect(mBounds) == false)
        return;

    // �뵱ǰ�ڵ�洢����ײ�����
    for (auto& co_wp : mColliders)
    {
        auto co = co_wp.lock();
        if (co.get() != &other &&   // ���ܶ��Լ���ײ���
            other.GetBounds().IsIntersect(co->GetBounds()))
        {
            other.OnCollisionCallback(*co);
            co->OnCollisionCallback(other);
        }
    }

    // ���ӽڵ��⣨�ݹ飩
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
