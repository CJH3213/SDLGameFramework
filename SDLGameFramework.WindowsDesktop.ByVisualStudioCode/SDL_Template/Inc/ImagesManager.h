#pragma once
#include <string>
#include "Texture.h"
#include <unordered_map>

class ImagesManager
{
private:
	// 单例模式下，禁用构造析构赋值
	ImagesManager() {};
	~ImagesManager() = default;
	ImagesManager(const ImagesManager&) = delete;
	ImagesManager& operator=(const ImagesManager&) = delete;
	// 获取输入类单例
	static ImagesManager& GetInstance();

private:
	//暂存图片
	std::unordered_map<std::string, Texture> mTextures;

	// 通过图片路径加载图片，缓存为纹理
	Texture LoadImage(std::string path);

public:
	static void Init();
	static void Close();

	static Texture ImageToTexture(std::string path);
};

