#include "ClickTest.h"
#include "Input.h"
#include "GameObject.h"
#include "Debug.h"

void ClickTest::OnMouseDown()
{
	Debug::Log(u8"OnMouseDown");
}

void ClickTest::OnMouseStay()
{
	Debug::Log(u8"OnMouseStay");
}

void ClickTest::OnMouseUp()
{
	Debug::Log(u8"OnMouseUp");
}

void ClickTest::OnClick()
{
	Debug::Log(u8"OnClick");
}

void ClickTest::Awake()
{
	mColliderCom = mGameObject->GetComponent<BoxCollider>();

	//(*mColliderCom).mMouseDownEvents.Add("ClickTest", std::bind(&ClickTest::OnMouseDown, this));
	//(*mColliderCom).mMouseDragEvents.Add("ClickTest", std::bind(&ClickTest::OnMouseStay, this));
	//(*mColliderCom).mMouseUpEvents.Add("ClickTest", std::bind(&ClickTest::OnMouseUp, this));
	//(*mColliderCom).OnClick.Add("ClickTest", std::bind(&ClickTest::OnClick, this));
}

void ClickTest::Update()
{
	
}
