#pragma once
#include "Texture.h"
#include "Color.h"
#include <unordered_map>
#include <map>
#include <SDL_ttf.h>

// ���Σ������߼���ͬʱʵ�ֶ�����ʽ��
enum FontGlyph
{
	Normal = 0x00,	//����
	Bold = 0x01,		//����
	Italic = 0x02,		//б��
	UnderLine = 0x04,	//�»���
	StrikeThrough = 0x08		//ɾ����
};

// ������ʽ
struct FontStyle
{
	std::string fontPath = "Resource/Fonts/simhei.ttf";	//�ֿ�����
	size_t size = 12;	//�����С
	Color color = Color::Black;	//������ɫ
	FontGlyph glyph = FontGlyph::Normal;
};

class FontManager
{
private:
	// ����ģʽ�£����ù���������ֵ
	FontManager() {};
	~FontManager() = default;
	FontManager(const FontManager&) = delete;
	FontManager& operator=(const FontManager&) = delete;
	// ��ȡ�����൥��
	static FontManager& GetInstance();

private:
	//�ݴ�����
	std::unordered_map<std::string, std::map<size_t, TTF_Font*>> mFontRegister;

	//ͨ���ֿ�·���������С�����ֿ�
	TTF_Font* LoadFont(std::string path, size_t ptsize);

public:
	static void Init();
	static void Close();
	// ���ַ�����������
	static Texture TextToTexture(std::u8string str, FontStyle style);
};

