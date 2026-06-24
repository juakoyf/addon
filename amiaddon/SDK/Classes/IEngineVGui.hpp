#pragma once
#include <cstdint>

enum VGuiPanel_t
{
	PANEL_ROOT = 0,
	PANEL_GAMEUIDLL,  // the console, game menu
	PANEL_CLIENTDLL,
	PANEL_TOOLS,
	PANEL_INGAMESCREENS,
	PANEL_GAMEDLL,
	PANEL_CLIENTDLL_TOOLS,
	PANEL_GAMEUIBACKGROUND, // the console background, shows under all other stuff in 3d engine view
	PANEL_TRANSITIONEFFECT,
	PANEL_STEAMOVERLAY,
};

class IEngineVGui {
public:
	virtual	~IEngineVGui(void) {}
	virtual uint32_t GetPanel(VGuiPanel_t type) = 0;
	virtual bool IsGameUIVisible() = 0;
	virtual void ActivateGameUI() = 0;
};