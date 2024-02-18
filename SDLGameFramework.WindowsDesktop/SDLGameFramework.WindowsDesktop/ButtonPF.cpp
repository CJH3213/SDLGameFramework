#include "ButtonPF.h"
#include "Button.h"
#include "ImagesManager.h"

void ButtonPF::Load()
{
	mName = "Button";

	// ��ӻ�����Ⱦ���
	auto com1 = std::make_shared<CanvasRenderer>();
	AddComponent(com1);

	// ��Ӻ�����ײ��
	auto com2 = std::make_shared<BoxCollider>();
	AddComponent(com2);

	// ��Ӱ�ť�ű����
	auto com3 = std::make_shared<Button>();
	AddComponent(com3);
	com3->SetNormalTex(ImagesManager::ImageToTexture("Resource/Images/button.png"));
	com3->SetmHighLigTex(ImagesManager::ImageToTexture("Resource/Images/button_hl.png"));
	com3->SetPressedTex(ImagesManager::ImageToTexture("Resource/Images/button_pres.png"));
}
