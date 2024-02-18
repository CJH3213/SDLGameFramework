#include "BlockTransPF.h"
#include "TextureRenderer.h"
#include "ImagesManager.h"

void BlockTransPF::Load()
{
	mName = "BlockTrans";

	// �����Ⱦ�����������1���Ƶ���Ļ
	auto com1 = std::make_shared<TextureRenderer>();
	this->AddComponent(com1);
	com1->SetTexture(ImagesManager::ImageToTexture("Resource/Images/Block_Trans.png"));
	com1->SetOrder(80);
}
