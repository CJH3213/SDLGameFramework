#include "CameraPF.h"
#include "TextureRenderer.h"
#include "Camera.h"
#include "WindowsManager.h"

#include "ClickTest.h"

void CameraPF::Load()
{
	// �����Ⱦ�����������1���Ƶ���Ļ
	//auto com1 = std::make_shared<TextureRenderer>();
	//this->AddComponent(com1);

	// �����������
	auto com2 = std::make_shared<Camera>();
	this->AddComponent(com2);
	com2->mIsMainCam = true;
	//com2->mScreenPos = { 0, 0 };		
	//com2->ResetSize({600,  400});

	// ��Ӳ��Խű�
	//auto com3 = std::make_shared<ClickTest>();
	//this->AddComponent(com3);
}
