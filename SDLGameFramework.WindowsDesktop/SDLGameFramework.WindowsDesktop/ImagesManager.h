#pragma once
#include <string>
#include "Texture.h"
#include <unordered_map>

class ImagesManager
{
private:
	// ����ģʽ�£����ù���������ֵ
	ImagesManager() {};
	~ImagesManager() = default;
	ImagesManager(const ImagesManager&) = delete;
	ImagesManager& operator=(const ImagesManager&) = delete;
	// ��ȡ�����൥��
	static ImagesManager& GetInstance();

private:
	//�ݴ�ͼƬ
	std::unordered_map<std::string, Texture> mTextures;

	// ͨ��ͼƬ·������ͼƬ������Ϊ����
	Texture LoadImage(std::string path);

public:
	static void Init();
	static void Close();

	static Texture ImageToTexture(std::string path);
};

