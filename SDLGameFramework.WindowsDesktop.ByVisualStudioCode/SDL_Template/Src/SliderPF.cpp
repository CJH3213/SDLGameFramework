#include "SliderPF.h"
#include "CanvasRenderer.h"
#include "Image.h"
#include "Scene.h"

class Background : public CanvasPrefab
{
public:virtual void Load() override
	{
		AddComponent(std::make_shared<CanvasRenderer>());
		AddComponent(std::make_shared<Image>());
	};
};

//class FillArea : public CanvasPrefab
//{
//public:virtual void Load() override {};
//};

class Fill : public CanvasPrefab
{
public:virtual void Load() override
	{
		AddComponent(std::make_shared<CanvasRenderer>());
		AddComponent(std::make_shared<Image>());
	};
};

//class HandleSlideArea : public CanvasPrefab
//{
//public:virtual void Load() override {};
//};

class Handle : public CanvasPrefab
{
public:virtual void Load() override
	{
		AddComponent(std::make_shared<CanvasRenderer>());
		AddComponent(std::make_shared<Image>());
	};
};


void SliderPF::Load()
{
	mName = "Slider";

	auto com1 = std::make_shared<Slider>();
	AddComponent(com1);

	auto background = std::make_shared<Background>();
	Instantiate(background, &GetTransform());

	auto fillArea = std::make_shared<CanvasPrefab>();
	Instantiate(fillArea, &GetTransform());

	auto fill = std::make_shared<Fill>();
	Instantiate(fill, &fillArea->GetTransform());

	auto handleSlideArea = std::make_shared<CanvasPrefab>();
	Instantiate(handleSlideArea, &GetTransform());

	auto handle = std::make_shared<Handle>();
	Instantiate(handle, &handleSlideArea->GetTransform());
}
