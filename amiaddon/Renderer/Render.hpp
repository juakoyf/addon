#pragma once
#include <string>
#include <sstream>
#include "../SDK/Interfaces/Interfaces.hpp"

enum text_flags : int {
	CENTER_X = (1 << 0),
	CENTER_Y = (1 << 1),
	ALIGN_RIGHT = (1 << 2),
	ALIGN_BOTTOM = (1 << 3),
	DROP_SHADOW = (1 << 4),
	OUTLINED = (1 << 5),
};

enum ETextures : int {
	LOGO,
	MAX,
};

namespace Render {
	namespace Engine {
		struct FontSize_t {
			int m_width;
			int m_height;
		};

		enum StringFlags_t {
			ALIGN_LEFT = 0,
			ALIGN_RIGHT,
			ALIGN_CENTER
		};

		class Font {
		public:
			HFont      m_handle;
			FontSize_t m_size;
			bool m_outline;

		public:
			__forceinline Font() : m_handle{ }, m_size{ }, m_outline{ } {};

			Font(const std::string& name, int s, int w, int flags);
			Font(HFont font);

			void string(int x, int y, Color color, const std::string& text, StringFlags_t flags = ALIGN_LEFT);
			void wstring(int x, int y, Color color, const std::wstring& text, StringFlags_t flags = ALIGN_LEFT);
			FontSize_t size(const std::string& text);
			FontSize_t wsize(const std::wstring& text);
		};

		extern Font esp;

		extern int m_width;
		extern int m_height;
		extern bool initialized;

		void Initialize();
		void InitFonts();
		void Invalidate();

		void Rect(int x, int y, int w, int h, Color color);
		void Rect(Vector2D pos, Vector2D size, Color color);
		void RectFilled(int x, int y, int w, int h, Color color);
		void RectFilled(Vector2D pos, Vector2D size, Color color);
		void Texture(const Vector2D& pos, const Vector2D& size, const ETextures texture, const Color tint);
	}

	Vector2D GetScreenSize();
};

