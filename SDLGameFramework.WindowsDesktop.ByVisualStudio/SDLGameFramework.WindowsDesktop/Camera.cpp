#include "Camera.h"
#include "WindowsManager.h"
#include "RenderManager.h"
#include "Transform.h"

//std::vector<ComponentPtr<Camera>> Camera::gMainCameras;


ComponentPtr<Camera> Camera::GetComponentPtr()
{
	return ComponentPtr<Camera>(mSelfPtr);
}

void Camera::ResetSize(Vector2 s)
{
	auto& win = WindowsManager::Get(0);
	mTexture = win.CreateTexture(s);
}

void Camera::Awake()
{
	ResetSize({ 500, 300 });

	auto cc = GetComponentPtr();
	RenderManager::AddCamera(cc);
}

void Camera::LateUpdate()
{
	if(mIsFullWin)
	{
		auto& win = WindowsManager::Get(0);
		auto newSize = win.GetSize();
		if (newSize != mTexture.mSize)
		{
			SDL_Log("ReSize: %f ; %f", newSize.X, newSize.Y);
			mWinPos = newSize / 2;
			ResetSize(newSize);
		}
	}
}

Vector2 Camera::WinToWorldPoint(const Vector2& winPos)
{
	Vector2 winToWorldPos = winPos - mWinPos;
	winToWorldPos.Y *= -1;	// 窗口Y轴和世界Y轴相反
	winToWorldPos += GetTransform().GetPosition();
	return winToWorldPos;
}

Vector2 Camera::WorldToWinPoint(const Vector2& worldPos)
{
	Vector2 worldToWinPos = worldPos - GetTransform().GetPosition();
	worldToWinPos.Y *= -1;		// 窗口Y轴和世界Y轴相反
	worldToWinPos += mWinPos;
	return worldToWinPos;
}

//void Camera::SetMainCam(bool b)
//{
//	if (!mIsMainCam && b)
//	{
//		gMainCameras.emplace_back(GetComponentPtr());
//	}
//	else if(mIsMainCam && !b)
//	{
//		auto ptr = GetComponentPtr();
//		auto it = std::find(gMainCameras.begin(), gMainCameras.end(), ptr);
//		if (it != gMainCameras.end())
//		{
//			gMainCameras.erase(it);
//		}
//	}
//	mIsMainCam = b;
//}
