#pragma once
#include "Component.h"
#include "Vector2.h"
#include "WaitInstruction.h"
#include "TetrisManager.h"
#include "Text.h"
#include "CollisionManager.h"
#include "BoxCollider.h"

class TetrisManager :
    public Component
{
private:
    bool shape_T[2][4] = { {1,1,1},{0,1,0} };
    bool shape_L[2][4] = { {1,1,1},{0,0,1} };
    bool shape_J[2][4] = { {1,1,1},{1,0,0} };
    bool shape_Z[2][4] = { {1,1,0},{0,1,1} };
    bool shape_S[2][4] = { {0,1,1},{1,1,0} };
    bool shape_I[2][4] = { {1,1,1,1} };
    bool(* shapes[6] )[4] = { shape_T, shape_L, shape_J, shape_Z, shape_S, shape_I };

    bool mIsOver = false;
    Vector2 mBlockSize = { 28, 28 };
    Vector2 mGridFirstPos = {0, -280};  // �������½Ǹ�����
    const int mY = 20, mX = 10;
    Transform* mBlocks[20][10];
    // ���䷽��ĸ�����
    std::shared_ptr<GameObject> mShapeGO;
    // �����ж���ײ����ʱ�ĸ�����
    std::shared_ptr<GameObject> mTempShapeGO;
    // ������ʾ��һ����״�ĸ�����
    std::shared_ptr<GameObject> mNextShapeGO;
    // �÷�
    int mScore = 0;
    ComponentPtr<Text> mScoreText;
    // ������ײ����
    std::vector<std::shared_ptr<BoxCollider>> mColliders;

    CoRet CoFun_WaitSec();
    bool PreCollisionCheck();
    bool CollisionCheck();
    void SyncShapeGO();     // ��ʵ�����䷽�����긴�Ƹ���ʱ���䷽��

    void GenerateShape();
    void GenerateBlocks(bool arr[2][4]);
    Vector2 GetGridPos(int x, int y);
    Transform** GetBlock(Vector2 v);
    // �����䷽��̶�
    void SolidifyBlocks();
    // ���ٷ���
    void Crush();

public:
    virtual void Start() override;
    virtual void Update() override;
    virtual void LateUpdate() override;
};

