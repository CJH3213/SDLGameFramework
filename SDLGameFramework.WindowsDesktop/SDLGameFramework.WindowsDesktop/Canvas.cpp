#include "Canvas.h"
#include "GameObject.h"
#include "RenderManager.h"
#include "WindowsManager.h"
#include "Debug.h"

void Canvas::Awake()
{
	mRectTrans = ComponentPtr<RectTransform>(GetTransform().mSelfPtr);
	(*mRectTrans).mPivot = { 0.5f, 0.5f };
}

void Canvas::Update()
{
	// 画布始终跟随摄像机位置，并跟随窗口大小
	auto mainCam = RenderManager::GetMainCamera();
	auto mainCamTrans = (*mainCam).GetTransform();
	Vector2 winSize = WindowsManager::Get(0).GetSize();

	GetTransform().SetPosition(mainCamTrans.GetPosition());
	GetTransform().SetRotation(mainCamTrans.GetRotation());
	(*mRectTrans).mSizeDelta = winSize - Vector2{1,1};

	//auto rect = (*mRectTrans).GetRectangle();
	//auto ltp = rect.GetLeftTopPos();
	//auto rbp = rect.GetRightBottomPos();

	//auto rect = (*mRectTrans).GetParentRect();
	//Debug::DrawRectangle(rect, Color::Orange);

	//rect = (*mRectTrans).GetAnchoredRect();
	//Debug::DrawRectangle(rect, Color::Blue);

	//rect = (*mRectTrans).GetRectangle();
	//Debug::DrawRectangle(rect, Color::Green);

	//Debug::DrawRectangle(rect, Color::Red);
}
