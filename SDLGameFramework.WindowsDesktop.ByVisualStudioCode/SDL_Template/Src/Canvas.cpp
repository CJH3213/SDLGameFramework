#include "Canvas.h"
#include "GameObject.h"
#include "RenderManager.h"
#include "WindowsManager.h"
#include "Debug.h"

void Canvas::SyncWithCamera()
{
	// 画布始终跟随摄像机位置，并跟随窗口大小
	auto mainCam = RenderManager::GetMainCamera();
	auto mainCamTrans = (*mainCam).GetTransform();
	Vector2 winSize = WindowsManager::Get(0).GetSize();

	GetTransform().SetPosition(mainCamTrans.GetPosition());
	GetTransform().SetRotation(mainCamTrans.GetRotation());
	(*mRectTrans).SetSizeDelta(winSize - Vector2{ 1,1 });
}

void Canvas::Awake()
{
	mRectTrans = ComponentPtr<RectTransform>(GetTransform().mSelfPtr);
	(*mRectTrans).SetPivot({ 0.5f, 0.5f });

	SyncWithCamera();
}

void Canvas::Update()
{
	SyncWithCamera();
}
