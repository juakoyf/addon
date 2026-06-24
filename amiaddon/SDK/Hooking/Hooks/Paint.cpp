#include "../Hooks.hpp"
#include "../../Interfaces/Interfaces.hpp"
#include "../Family/Family.hpp"
#include "../Renderer/Render.hpp"

typedef void(__thiscall* StartDrawing_t)(void*);
typedef void(__thiscall* FinishDrawing_t)(void*);

static StartDrawing_t StartDrawFn = (StartDrawing_t)Memory::Scan("vguimatsurface.dll", "55 8B EC 83 E4 C0 83 EC 38");
static FinishDrawing_t EndDrawFn = (FinishDrawing_t)Memory::Scan("vguimatsurface.dll", "8B 0D ? ? ? ? 56 C6 05");

void __stdcall Hooked::hkPaint(int mode)
{
	oPaint(g_pEngineVGui.Get(), mode);


	if (mode & 1) {
		StartDrawFn(g_pSurface.Get());

		Render::Engine::Initialize();

		if (!Render::Engine::m_height || !Render::Engine::m_width) {
			g_pSurface->GetScreenSize(Render::Engine::m_width, Render::Engine::m_height);
		}
		else {
			static int width = Render::Engine::m_width;
			static int height = Render::Engine::m_height;

			g_pSurface->GetScreenSize(Render::Engine::m_width, Render::Engine::m_height);

			if (width != Render::Engine::m_width || height != Render::Engine::m_height) {
				Render::Engine::InitFonts();

				width = Render::Engine::m_width;
				height = Render::Engine::m_height;
			}
		}

		Family::OnPaint();

		if (Family::bWatermark && g_pEngine->IsInGame()) {

			int x = Render::Engine::m_width - 32 - 10;
			int y = 10;
			float flAlpha = 80 + (std::sin(g_pGlobalVars->curtime * 2.f) * 0.5f + 0.5f) * 175;
			Render::Engine::Texture(Vector2D(x, y), Vector2D(32, 32), ETextures::LOGO, Color(255, 255, 255, flAlpha));
		}

		EndDrawFn(g_pSurface.Get());

	}

}
