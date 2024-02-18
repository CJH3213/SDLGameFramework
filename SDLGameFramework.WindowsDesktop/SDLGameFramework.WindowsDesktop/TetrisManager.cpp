#include "TetrisManager.h"
#include "RenderManager.h"
#include "Input.h"
#include "Scene.h"
#include "BlockPF.h"
#include "BlockTransPF.h"
#include "Debug.h"
#include "Random.h"
#include "ScorePF.h"
#include "NextText.h"
#include "ColliderTestPF.h"
#include "SceneManager.h"
#include "TestScene.h"


void TetrisManager::GenerateShape()
{
	mShapeGO = std::make_shared<GameObject>();
	Instantiate(mShapeGO);
	//mShapeGO->GetTransform().SetPosition({0, 280});

	mTempShapeGO = std::make_shared<GameObject>();
	Instantiate(mTempShapeGO);

	mNextShapeGO = std::make_shared<GameObject>();
	Instantiate(mNextShapeGO);
	mNextShapeGO->GetTransform().SetPosition({ -300, 0 });
}

void TetrisManager::GenerateBlocks(bool arr[2][4])
{
	Vector2 pos = Vector2::Zero;

	// 产生新下落方块后，父对象要归位至上方
	mShapeGO->GetTransform().SetPosition(GetGridPos(5, mY+1));

	// 把预测方块赋予到当前方块上
	DestroyAllChildren(*mShapeGO);
	DestroyAllChildren(*mTempShapeGO);
	auto& blocks = mNextShapeGO->GetTransform().GetChildren();
	for (auto trans : blocks)
	{
		auto gobj1 = std::make_shared<BlockPF>();
		Instantiate(gobj1, &(mShapeGO->GetTransform()));
		gobj1->GetTransform().mLocalPosition = trans->mLocalPosition;

		auto gobj2 = std::make_shared<BlockTransPF>();
		Instantiate(gobj2, &(mTempShapeGO->GetTransform()));
		gobj2->GetTransform().mLocalPosition = trans->mLocalPosition;
	}

	// 装载预测方块
	DestroyAllChildren(*mNextShapeGO);
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (arr[i][j])
			{
				auto gobj3 = std::make_shared<BlockTransPF>();
				Instantiate(gobj3, &(mNextShapeGO->GetTransform()));
				gobj3->GetTransform().mLocalPosition = pos;
			}
			pos.X += mBlockSize.X;
		}
		pos.X = 0;
		pos.Y += mBlockSize.Y;
	}
}

CoRet TetrisManager::CoFun_WaitSec()
{
	while (true)
	{

		Yield WaitForSeconds(1);
		// 每隔1秒下落一格
		if(mIsOver == false)
		{

			SyncShapeGO();
			Vector2 p = mTempShapeGO->GetTransform().GetPosition();
			p.Y -= mBlockSize.Y;
			mTempShapeGO->GetTransform().SetPosition(p);
			if (PreCollisionCheck() == false)
				mShapeGO->GetTransform().SetPosition(p);
			else
			{
				SolidifyBlocks();
				Crush();
				if (mIsOver == false)
					GenerateBlocks(shapes[Random::Range(0, 5)]);
			}

			//SDL_Log("Trans Y: %f", mShapeGO->GetTransform().mLocalPosition.Y);
		}
	}

	co_return 0;
}

bool TetrisManager::PreCollisionCheck()
{
	for (auto childTrans : mTempShapeGO->GetTransform().GetChildren())
	{
		Vector2 pos = childTrans->GetPosition();

		// 在棋盘上方不作碰撞检测
		if (pos.Y > GetGridPos(0, mY - 1).Y && 
			pos.X >= GetGridPos(0, 0).X && 
			pos.X <=GetGridPos(mX-1,0).X)
			continue;

		// 不在棋盘内，或者碰块了
		Transform** belowTrans = GetBlock(pos);
		if (belowTrans == nullptr || *belowTrans != nullptr)	
			return true;
	}

	return false;
}

bool TetrisManager::CollisionCheck()
{
	bool isCollided = false;

	// 踢墙校正
	for (auto childTrans : mShapeGO->GetTransform().GetChildren())
	{
		Vector2 pos = childTrans->GetPosition();

		// 踢左墙
		if (pos.X < mGridFirstPos.X)
		{
			Vector2 shapePos = mShapeGO->GetTransform().GetPosition();
			shapePos.X += mGridFirstPos.X - pos.X;
			mShapeGO->GetTransform().SetPosition(shapePos);
		}

		// 踢右墙
		if (pos.X > mGridFirstPos.X + mBlockSize.X * (mX-1))
		{
			SDL_Log("Kick R : %f  ,  %f", pos.X, mGridFirstPos.X + mBlockSize.X * (mX-1));
			Vector2 shapePos = mShapeGO->GetTransform().GetPosition();
			shapePos.X += mGridFirstPos.X + mBlockSize.X * (mX-1) - pos.X;
			mShapeGO->GetTransform().SetPosition(shapePos);
		}
	}

	// 与地板或方块碰撞
	for (auto childTrans : mShapeGO->GetTransform().GetChildren())
	{
		Vector2 pos = childTrans->GetPosition();

		if(pos.Y <= mGridFirstPos.Y)	// 下方无格子，碰底了
		{
			//pos.Y = mGridFirstPos.Y;
			//childTrans->SetPosition(pos);
			isCollided = true;
			break;
		}

		// 每个下落块的下方是否已经有方块
		Vector2 belowPos = pos;
		belowPos.Y -= mBlockSize.Y;
		Transform** belowTrans = GetBlock(belowPos);
		if(belowTrans && *belowTrans != nullptr)	// 下方有格子，且格子有方块，碰块了
		{
			isCollided = true;
			break;
		}
	}

	// Bug：横向移动时会穿插到其他方块内

	return isCollided;
}

void TetrisManager::SyncShapeGO()
{
	auto& trans1 = mShapeGO->GetTransform();
	auto& trans2 = mTempShapeGO->GetTransform();

	trans2.SetPosition(trans1.GetPosition());
	trans2.SetRotation(trans1.GetRotation());
}

Vector2 TetrisManager::GetGridPos(int x, int y)
{
	return mGridFirstPos + mBlockSize * Vector2((float)x, (float)y);
}

Transform** TetrisManager::GetBlock(Vector2 v)
{
	v -= mGridFirstPos;
	v /= mBlockSize;

	// 四舍五入，正负有区别
	v.X = v.X >= 0.0f ? v.X + 0.5f : v.X - 0.5f;
	v.Y = v.Y >= 0.0f ? v.Y + 0.5f : v.Y - 0.5f;

	int y = (int)v.Y;
	int x = (int)v.X;

	if (y < 0 || y >= mY || x < 0 || x >= mX)
		return nullptr;

	return &(mBlocks[y][x]);
}

void TetrisManager::Awake()
{
	GenerateShape();
	// 第一次产生预测方块，第二次将预测方块移至当前方块
	GenerateBlocks(shapes[Random::Range(0, 5)]);
	GenerateBlocks(shapes[Random::Range(0, 5)]);

	// 分数
	auto gobj1 = std::make_shared<ScorePF>();
	Instantiate(gobj1);
	gobj1->GetTransform().SetPosition({ - 300, +200});
	mScoreText = gobj1->GetComponent<Text>();

	// 下一个
	auto gobj2 = std::make_shared<NextText>();
	Instantiate(gobj2);
	gobj2->GetTransform().SetPosition({ -300, 100 });

	StartCoroutine(CoFun_WaitSec());
}

void TetrisManager::SolidifyBlocks()
{
 	auto& cs = mShapeGO->GetTransform().GetChildren();
	for (; !cs.empty();  )
	{
		auto& childTrans = *(cs.end()-1);

		Vector2 pos = childTrans->GetPosition();

		Transform** pTrans = GetBlock(pos);
		if (pTrans == nullptr)	// 无法把方块放置在棋盘上时，棋盘已满
		{
			mIsOver = true;
			DestroyAllChildren(*mShapeGO);
			SDL_Log("IsOver");
			return;
		}

		childTrans->SetParent(nullptr);
		childTrans->SetPosition(pos);

		*pTrans = childTrans;
		mBlocks;
	}
}

void TetrisManager::Update()
{
	// 切换场景
	if (Input::GetKeyDown(SDL_SCANCODE_P))
	{
		std::shared_ptr<Scene> newScene = std::make_shared<TestScene>();
		SceneManager::LoadScene("Scene02", newScene);
	}

	// 绘制棋盘
	for(int y=0; y<mY; y++)
		for(int x=0; x<mX; x++)
		{
			Color c = mBlocks[y][x] ? Color::Green : Color::Red;
			Vector2 pos = mGridFirstPos + mBlockSize * Vector2((float)x, (float)y);
			Debug::DrawLine(pos, pos, c);
		}

	if (Input::GetKeyDown(SDL_SCANCODE_W))
	{
		// 先用临时方块测试下一步是否碰撞
		SyncShapeGO();
		float r = mTempShapeGO->GetTransform().GetRotation();
		mTempShapeGO->GetTransform().SetRotation(r + 90);
		// 如果预判到没有碰撞，再更改真实方块的角度
		if(PreCollisionCheck() == false)
			mShapeGO->GetTransform().SetRotation(r + 90);
	}

	if (Input::GetKeyDown(SDL_SCANCODE_A))
	{
		SyncShapeGO();
		Vector2 p = mTempShapeGO->GetTransform().GetPosition();
		p.X -= mBlockSize.X;
		mTempShapeGO->GetTransform().SetPosition(p);
		if (PreCollisionCheck() == false)
			mShapeGO->GetTransform().SetPosition(p);
	}

	if (Input::GetKeyDown(SDL_SCANCODE_D))
	{
		SyncShapeGO();
		Vector2 p = mTempShapeGO->GetTransform().GetPosition();
		p.X += mBlockSize.X;
		mTempShapeGO->GetTransform().SetPosition(p);
		if (PreCollisionCheck() == false)
			mShapeGO->GetTransform().SetPosition(p);
	}

	if (Input::GetKeyDown(SDL_SCANCODE_S))
	{
		SyncShapeGO();
		Vector2 p = mTempShapeGO->GetTransform().GetPosition();
		p.Y -= mBlockSize.Y;
		mTempShapeGO->GetTransform().SetPosition(p);
		if (PreCollisionCheck() == false)
			mShapeGO->GetTransform().SetPosition(p);
	}

	// 直接降落到底
	if (Input::GetKeyDown(SDL_SCANCODE_SPACE))
	{
		SyncShapeGO();
		Vector2 p = mTempShapeGO->GetTransform().GetPosition();
		do
		{
			p.Y -= mBlockSize.Y;
			mTempShapeGO->GetTransform().SetPosition(p);
		} while (PreCollisionCheck() == false);
		p.Y += mBlockSize.Y;
		mShapeGO->GetTransform().SetPosition(p);
	}

}

void TetrisManager::Crush()
{
	for (int y = 0; y < mY;)
	{
		// 核算当前一行多少个方块
		int count = 0;
		for (int x = 0; x < mX; x++)
		{
			if (mBlocks[y][x] == nullptr)
				break;
			count++;
		}

		if (count < mX)
		{
			// 没有销毁当前行才需要y++，
			// 如果销毁当前行，上一行落下落，继续在当前y判断
			y++;		
			continue;
		}

		//如果一行方块满了
		// 销毁当前行方块
		for (int x = 0; x < mX; x++)
		{
			Destroy(*(mBlocks[y][x]->mGameObject));
			mBlocks[y][x] = nullptr;
		}

		// 更新分数
		mScore += mX;
		auto sc = std::to_string(mScore);
		(*mScoreText).SetContent(std::u8string(u8"分数: ") + (const char8_t*)sc.c_str());

		// 当前行上方所有方块下移一格
		for (int y2 = y + 1; y2 < mY; y2++)
		{
			for (int x = 0; x < mX; x++)
			{
				Transform* trans = mBlocks[y2][x];
				if (trans != nullptr)
				{
					Vector2 pos = trans->GetPosition();
					pos.Y -= mBlockSize.Y;
					trans->SetPosition(pos);
					mBlocks[y2][x] = nullptr;
					mBlocks[y2 - 1][x] = trans;
				}
			}
		}

	}
}

void TetrisManager::LateUpdate()
{
	// 检测碰撞
	//if (CollisionCheck())

}
