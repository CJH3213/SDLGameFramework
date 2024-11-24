#include "MusicPlayerScene.h"
#include "CameraPF.h"
#include "MusicPlayScript.h"
#include "CanvasPF.h"
#include "Image.h"

class CoverImage : public CanvasPrefab
{
public:virtual void Load() override
{
	mName = "CoverImage";

	auto rectTrans = ComponentPtr<RectTransform>(GetTransform().mSelfPtr);
	(*rectTrans).SetAnchorMin({ 0.5f, 0.5f });
	(*rectTrans).SetAnchorMax({ 0.5f, 0.5f });
	(*rectTrans).SetAnchoredPosition({ 0.0f, 0.0f });
	(*rectTrans).SetPivot({ 0.5f, 0.5f });
	(*rectTrans).SetSizeDelta({ 310, 310 });
	// ���У� UI���κ㶨310x310

	AddComponent(std::make_shared<CanvasRenderer>());
	AddComponent(std::make_shared<Image>());
};
};

void MusicPlayerScene::Load()
{
	// ��������1����Ϊ���������
	auto gobj1 = std::make_shared<CameraPF>();
	AddGObj(gobj1);

	// ��������3����ΪUI����
	auto gobj2 = std::make_shared<CanvasPF>();
	AddGObj(gobj2);

	// ��������4����ΪUIͼƬ
	auto gobj2_1 = std::make_shared<CoverImage>();
	AddGObj(gobj2_1);
	gobj2_1->GetTransform().SetParent(&gobj2->GetTransform());

	auto gobj3 = std::make_shared<MusicPlayPF>();
	AddGObj(gobj3);
}
