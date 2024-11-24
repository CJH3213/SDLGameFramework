#pragma once
#include <vector>
#include <memory>
#include "TextureRenderer.h"
#include "Camera.h"

class RenderManager
{
private:
	// ����ģʽ�£����ù���������ֵ
	RenderManager() {};
	~RenderManager() = default;
	RenderManager(const RenderManager&) = delete;
	RenderManager& operator=(const RenderManager&) = delete;
	// ��ȡ�����൥��
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

