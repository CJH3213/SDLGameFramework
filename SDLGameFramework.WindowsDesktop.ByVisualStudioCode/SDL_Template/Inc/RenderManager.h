#pragma once
#include <vector>
#include <memory>
#include "TextureRenderer.h"
#include "Camera.h"

class RenderManager
{
private:
	// 单例模式下，禁用构造析构赋值
	RenderManager() {};
	~RenderManager() = default;
	RenderManager(const RenderManager&) = delete;
	RenderManager& operator=(const RenderManager&) = delete;
	// 获取输入类单例
	static RenderManager& GetInstance();

	std::vector<std::weak_ptr<RenderInterface>> mRenderers;
	std::vector<ComponentPtr<Camera>> mCameras;

public:

	static void AddRenderer(std::weak_ptr<RenderInterface> wp);
	static void AddCamera(ComponentPtr<Camera> comPtr);
	static ComponentPtr<Camera> GetMainCamera();
	static void ChangeOrder(RenderInterface& rif, int order);

	static void Update();
};

