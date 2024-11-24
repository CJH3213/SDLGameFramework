#include "TetrisGameScene.h"
#include "CameraPF.h"
#include "AxisXYPF.h"
#include "TetrisManagerPF.h"
#include "CanvasPF.h"


void TetrisGameScene::Load()
{
	// ��������1����Ϊ���������
	auto gobj1 = std::make_shared<CameraPF>();
	AddGObj(gobj1);

	// ��������3����ΪUI����
	auto gobj2 = std::make_shared<CanvasPF>();
	AddGObj(gobj2);

	// �÷��ı�
	auto gobj2_1 = std::make_shared<ScoreTextPF>();
	AddGObj(gobj2_1);
	gobj2_1->GetTransform().SetParent(&gobj2->GetTransform());

	// ��һ����״�ı�
	auto gobj2_2 = std::make_shared<NextTextPF>();
	AddGObj(gobj2_2);
	gobj2_2->GetTransform().SetParent(&gobj2->GetTransform());

	// ��һ����״�ĸ�����
	auto gobj2_3 = std::make_shared<NextShapePF>();
	AddGObj(gobj2_3);
	gobj2_3->GetTransform().SetParent(&gobj2->GetTransform());

	// ��������2����Ϊ������ͼƬ����
	auto gobj3 = std::make_shared<AxisXYPF>();
	AddGObj(gobj3);

	// ��������3����Ϊ����˹������Ϸ������
	auto gobj4 = std::make_shared<TetrisManagerPF>();
	AddGObj(gobj4);

	// MS�༭����Ĭ��GBK���룬�����ֽ�һ���֣�SDLֻ����UTF8���룬���ֽ�һ����
	std::u8string wavPath = u8"Resource/Sounds/�ܽ���-����.ape";
	mWav1 = AudioManager::LoadAudioFile(std::string(wavPath.cbegin(), wavPath.cend()));
	AudioManager::Play(mWav1);

	//wavPath = u8"Resource/Sounds/�ֿ��� ��ɯ-���紵��������.ape";
	//mWav2 = AudioManager::LoadAudioFile(std::string(wavPath.cbegin(), wavPath.cend()));
	//AudioManager::Play(mWav2);
}

#include "Text.h"
void ScoreTextPF::Load()
{
	mName = "ScoreText";

	auto rectTrans = ComponentPtr<RectTransform>(GetTransform().mSelfPtr);
	(*rectTrans).SetAnchorMin({ 0.0f, 0.5f });
	(*rectTrans).SetAnchorMax({ 0.0f, 0.5f });
	(*rectTrans).SetAnchoredPosition({ 100.0f, 120.0f });

	// ��ӻ�����Ⱦ���
	auto com1 = std::make_shared<CanvasRenderer>();
	AddComponent(com1);

	// ����ı�UI���
	auto com2 = std::make_shared<Text>();
	AddComponent(com2);

	com2->SetContent(u8"�÷֣�0");
	com2->SetFontSize(36);
}

void NextTextPF::Load()
{
	mName = "NextText";

	auto rectTrans = ComponentPtr<RectTransform>(GetTransform().mSelfPtr);
	(*rectTrans).SetAnchorMin({ 0.0f, 0.5f });
	(*rectTrans).SetAnchorMax({ 0.0f, 0.5f });
	(*rectTrans).SetAnchoredPosition({ 100.0f, 60.0f });

	// ��ӻ�����Ⱦ���
	auto com1 = std::make_shared<CanvasRenderer>();
	AddComponent(com1);

	// ����ı�UI���
	auto com2 = std::make_shared<Text>();
	AddComponent(com2);

	com2->SetContent(u8"��һ����״��");
	com2->SetFontSize(36);
}

void NextShapePF::Load()
{
	mName = "NextShape";

	auto rectTrans = ComponentPtr<RectTransform>(GetTransform().mSelfPtr);
	(*rectTrans).SetAnchorMin({ 0.0f, 0.5f });
	(*rectTrans).SetAnchorMax({ 0.0f, 0.5f });
	//(*rectTrans).SetAnchoredPosition({ 100.0f, 00.0f });
	(*rectTrans).SetInsetAndSizeFromParentEdge(RectTransform::Edge::Left, 50, 100);
}
