#include "RectanglePF.h"
#include "TextureRenderer.h"

void RectanglePF::Load()
{
	// �趨����1�Ļ����任����
	//(*gobj1).GetTransform().SetRotation(10);
	this->GetTransform().SetPosition({ 0, 0 });
	//(*gobj1).GetTransform().SetScale({ 2, 2 });

	// �����Ⱦ�����������1���Ƶ���Ļ
	auto com1 = std::make_shared<TextureRenderer>();
	this->AddComponent(com1);
}
