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

	// ���������䷽��󣬸�����Ҫ��λ���Ϸ�
	mShapeGO->GetTransform().SetPosition(GetGridPos(5, mY+1));

	// ��Ԥ�ⷽ�鸳�赽��ǰ������
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

	// װ��Ԥ�ⷽ��
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
		// ÿ��1������һ��
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

		// �������Ϸ�������ײ���
		if (pos.Y > GetGridPos(0, mY - 1).Y && 
			pos.X >= GetGridPos(0, 0).X && 
			pos.X <=GetGridPos(mX-1,0).X)
			continue;

		// ���������ڣ�����������
		Transform** belowTrans = GetBlock(pos);
		if (belowTrans == nullptr || *belowTrans != nullptr)	
			return true;
	}

	return false;
}

bool TetrisManager::CollisionCheck()
{
	bool isCollided = false;

	// ��ǽУ��
	for (auto childTrans : mShapeGO->GetTransform().GetChildren())
	{
		Vector2 pos = childTrans->GetPosition();

		// ����ǽ
		if (pos.X < mGridFirstPos.X)
		{
			Vector2 shapePos = mShapeGO->GetTransform().GetPosition();
			shapePos.X += mGridFirstPos.X - pos.X;
			mShapeGO->GetTransform().SetPosition(shapePos);
		}

		// ����ǽ
		if (pos.X > mGridFirstPos.X + mBlockSize.X * (mX-1))
		{
			SDL_Log("Kick R : %f  ,  %f", pos.X, mGridFirstPos.X + mBlockSize.X * (mX-1));
			Vector2 shapePos = mShapeGO->GetTransform().GetPosition();
			shapePos.X += mGridFirstPos.X + mBlockSize.X * (mX-1) - pos.X;
			mShapeGO->GetTransform().SetPosition(shapePos);
		}
	}

	// ��ذ�򷽿���ײ
	for (auto childTrans : mShapeGO->GetTransform().GetChildren())
	{
		Vector2 pos = childTrans->GetPosition();

		if(pos.Y <= mGridFirstPos.Y)	// �·��޸��ӣ�������
		{
			//pos.Y = mGridFirstPos.Y;
			//childTrans->SetPosition(pos);
			isCollided = true;
			break;
		}

		// ÿ���������·��Ƿ��Ѿ��з���
		Vector2 belowPos = pos;
		belowPos.Y -= mBlockSize.Y;
		Transform** belowTrans = GetBlock(belowPos);
		if(belowTrans && *belowTrans != nullptr)	// �·��и��ӣ��Ҹ����з��飬������
		{
			isCollided = true;
			break;
		}
	}

	// Bug�������ƶ�ʱ�ᴩ�嵽����������

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

	// �������룬����������
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
	// ��һ�β���Ԥ�ⷽ�飬�ڶ��ν�Ԥ�ⷽ��������ǰ����
	GenerateBlocks(shapes[Random::Range(0, 5)]);
	GenerateBlocks(shapes[Random::Range(0, 5)]);

	// ����
	auto gobj1 = std::make_shared<ScorePF>();
	Instantiate(gobj1);
	gobj1->GetTransform().SetPosition({ - 300, +200});
	mScoreText = gobj1->GetComponent<Text>();

	// ��һ��
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
		if (pTrans == nullptr)	// �޷��ѷ��������������ʱ����������
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
	// �л�����
	if (Input::GetKeyDown(SDL_SCANCODE_P))
	{
		std::shared_ptr<Scene> newScene = std::make_shared<TestScene>();
		SceneManager::LoadScene("Scene02", newScene);
	}

	// ��������
	for(int y=0; y<mY; y++)
		for(int x=0; x<mX; x++)
		{
			Color c = mBlocks[y][x] ? Color::Green : Color::Red;
			Vector2 pos = mGridFirstPos + mBlockSize * Vector2((float)x, (float)y);
			Debug::DrawLine(pos, pos, c);
		}

	if (Input::GetKeyDown(SDL_SCANCODE_W))
	{
		// ������ʱ���������һ���Ƿ���ײ
		SyncShapeGO();
		float r = mTempShapeGO->GetTransform().GetRotation();
		mTempShapeGO->GetTransform().SetRotation(r + 90);
		// ���Ԥ�е�û����ײ���ٸ�����ʵ����ĽǶ�
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

	// ֱ�ӽ��䵽��
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
		// ���㵱ǰһ�ж��ٸ�����
		int count = 0;
		for (int x = 0; x < mX; x++)
		{
			if (mBlocks[y][x] == nullptr)
				break;
			count++;
		}

		if (count < mX)
		{
			// û�����ٵ�ǰ�в���Ҫy++��
			// ������ٵ�ǰ�У���һ�������䣬�����ڵ�ǰy�ж�
			y++;		
			continue;
		}

		//���һ�з�������
		// ���ٵ�ǰ�з���
		for (int x = 0; x < mX; x++)
		{
			Destroy(*(mBlocks[y][x]->mGameObject));
			mBlocks[y][x] = nullptr;
		}

		// ���·���
		mScore += mX;
		auto sc = std::to_string(mScore);
		(*mScoreText).SetContent(std::u8string(u8"����: ") + (const char8_t*)sc.c_str());

		// ��ǰ���Ϸ����з�������һ��
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
	// �����ײ
	//if (CollisionCheck())

}
