#define NOMINMAX
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x)
#include "../Libraries/stb/stb_image.h"
#include "Render.hpp"
#include <array>
#include <algorithm>
#include "Textures/Textures.h"

Render::Engine::Font::Font(const std::string& name, int s, int w, int flags) {
	m_handle = g_pSurface->CreateFont_();

	const auto actual_lang = g_pFontManager->GetLanguage();

	g_pFontManager->GetLanguage() = const_cast<char*>("english");
	g_pSurface->SetFontGlyphSet(m_handle, name.data(), s, w, 0, 0, flags);
	g_pFontManager->GetLanguage() = actual_lang;

	m_size = size("A");
}

Render::Engine::Font::Font(HFont font) {
	m_handle = font;
	m_size = size("A");
}

void Render::Engine::Font::string(int x, int y, Color color, const std::string& text, StringFlags_t flags) {
	int __len = MultiByteToWideChar(CP_UTF8, 0, text.data(), (int)text.size(), nullptr, 0);

	std::wstring __wtext(__len, 0);

	MultiByteToWideChar(CP_UTF8, 0, text.data(), (int)text.size(), &__wtext[0], __len);

	wstring(x, y, color, __wtext, flags);
}

void Render::Engine::Font::wstring(int x, int y, Color color, const std::wstring& text, StringFlags_t flags) {
	int w, h;

	g_pSurface->GetTextSize(m_handle, text.c_str(), w, h);
	g_pSurface->DrawSetTextFont(m_handle);
	g_pSurface->DrawSetTextColor(color);

	if (flags & Render::Engine::ALIGN_RIGHT)
		x -= w;

	if (flags & Render::Engine::ALIGN_CENTER)
		x -= w / 2;

	g_pSurface->DrawSetTextPos(x, y);
	g_pSurface->DrawPrintText(text.c_str(), (int)text.size());
}

Render::Engine::FontSize_t Render::Engine::Font::size(const std::string& text) {
	if (text.empty())
		return {};

	int len = MultiByteToWideChar(CP_UTF8, 0, text.data(), (int)text.size(), nullptr, 0);

	std::wstring wtext(len, 0);

	MultiByteToWideChar(CP_UTF8, 0, text.data(), (int)text.size(), &wtext[0], len);

	return wsize(wtext);
}

Render::Engine::FontSize_t Render::Engine::Font::wsize(const std::wstring& text) {
	if (text.empty())
		return {};

	FontSize_t res;
	g_pSurface->GetTextSize(m_handle, text.data(), res.m_width, res.m_height);
	return res;
}

std::array<uint32_t, static_cast<int>(ETextures::MAX)> textures;
uint32_t& GetTexture(const ETextures texture) {
	return textures[static_cast<int>(texture)];
}

void MakeTextureFromImage(const ETextures texture, const uint8_t* png_data, size_t size, const int32_t width, const int32_t height) {
	int image_width, image_height, channels;

	stbi_set_flip_vertically_on_load(false);
	stbi_set_flip_vertically_on_load_thread(false);
	const auto image_data = stbi_load_from_memory(png_data, size, &image_width, &image_height, &channels, 4);

	if (image_data == nullptr)
		return;

	g_pSurface->UpdateTexture(GetTexture(texture), image_data, image_width, image_height);
	stbi_image_free(image_data);
}

namespace Render::Engine {
	Font esp;

	int m_width;
	int m_height;
	bool initialized;
}

void Render::Engine::Initialize() {
	if (initialized)
		return;

	InitFonts();

	g_pSurface->GetScreenSize(m_width, m_height);

	for (int texture = 0; texture < ETextures::MAX; ++texture) {
		GetTexture((ETextures)texture) = g_pSurface->CreateNewTextureID(true);
	}

	MakeTextureFromImage(ETextures::LOGO, watermarkImage, sizeof(watermarkImage), 32, 32);

	ConsolePrint("Render system initialized (width: %d, height: %d)", m_width, m_height);

	initialized = true;
}

void Render::Engine::InitFonts() {
	esp = Font("Tahoma", 16, FW_NORMAL, FONTFLAG_ANTIALIAS);
}

void Render::Engine::Invalidate() {
	initialized = false;
}

Vector2D Render::GetScreenSize() {
	return Vector2D(Render::Engine::m_width, Render::Engine::m_height);
}

void Render::Engine::Rect(int x, int y, int w, int h, Color color) {
	g_pSurface->DrawSetColor(color);
	g_pSurface->DrawOutlinedRect(x, y, x + w, y + h);
}

void Render::Engine::Rect(Vector2D pos, Vector2D size, Color color) {
	Render::Engine::Rect(pos.x, pos.y, size.x, size.y, color);
}

void Render::Engine::RectFilled(int x, int y, int w, int h, Color color) {
	g_pSurface->DrawSetColor(color);
	g_pSurface->DrawFilledRect(x, y, x + w, y + h);
}

void Render::Engine::RectFilled(Vector2D pos, Vector2D size, Color color) {
	Render::Engine::RectFilled(pos.x, pos.y, size.x, size.y, color);
}

void Render::Engine::Texture(const Vector2D& pos, const Vector2D& size, const ETextures texture, const Color tint) {
	g_pSurface->DrawSetColor(tint);
	g_pSurface->DrawSetTexture(GetTexture(texture));
	g_pSurface->DrawTexturedRect(pos.x, pos.y, pos.x + size.x, pos.y + size.y);
}