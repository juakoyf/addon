#include "Interfaces.hpp"
#include "../Classes/CHudChat.hpp"
#include "../../Utils/PatternScan.hpp"
#include "../../Utils/ConsoleLog.hpp"

Interfaces g_Interfaces;

Ptr<CBaseHudChat> g_pChat = nullptr;
Ptr<IEngineVGui> g_pEngineVGui = nullptr;
Ptr<ISurface> g_pSurface = nullptr;
Ptr<CFontManager> g_pFontManager = nullptr;
Ptr<IBaseClientDLL> g_pClient = nullptr;
Ptr<IGameEventManager> g_pGameEvent = nullptr;
Ptr<IVEngineClient> g_pEngine = nullptr;
Ptr<IClientEntityList> g_pEntityList = nullptr;
Ptr<CGlobalVars> g_pGlobalVars = nullptr;
Ptr<CClientState> g_pClientState = nullptr;
Ptr<CCSGameRules> g_pGameRules = nullptr;

bool Interfaces::Create(void* lpReserved) {

    g_pClient = reinterpret_cast<IBaseClientDLL*>(CreateInterface("client.dll", "VClient018"));
    if (!g_pClient.IsValid()) {
        ConsolePrint("Failed to initialize g_pClient");
        return false;
    }
    ConsolePrint("g_pClient initialized");

    g_pClientState = **reinterpret_cast<CClientState***>(Memory::Scan("engine.dll", "A1 ? ? ? ? 8B 80 ? ? ? ? C3") + 1);
    if (!g_pClientState.IsValid()) {
        ConsolePrint("Failed to initialize g_pClientState");
        return false;
    }
    ConsolePrint("g_pClientState initialized");


    g_pGlobalVars = (**reinterpret_cast<CGlobalVars***>((*reinterpret_cast<uintptr_t**>(g_pClient.Get()))[11] + 10));
    if (!g_pGlobalVars.IsValid()) {
        ConsolePrint("Failed to initialize g_pGlobalVars");
        return false;
    }
    ConsolePrint("g_pGlobalVars initialized");

    g_pGameEvent = reinterpret_cast<IGameEventManager*>(CreateInterface("engine.dll", "GAMEEVENTSMANAGER002"));
    if (!g_pGameEvent.IsValid()) {
        ConsolePrint("Failed to initialize g_pGameEvent");
        return false;
    }
    ConsolePrint("g_pGameEvent initialized");

    g_pChat = *reinterpret_cast<CBaseHudChat**>(*(uintptr_t*)(Memory::Scan("client.dll", "8B 3D ?? ?? ?? ?? 8D 4C 24 38") + 2));
    if (!g_pChat.IsValid()) {
        ConsolePrint("Failed to initialize g_pChat");
        return false;
    }
    ConsolePrint("g_pChat initialized");

    g_pEngineVGui = reinterpret_cast<IEngineVGui*>(CreateInterface("engine.dll", "VEngineVGui001"));
    if (!g_pEngineVGui.IsValid()) {
        ConsolePrint("Failed to initialize g_pEngineVGui");
        return false;
    }
    ConsolePrint("g_pEngineVGui initialized");

    g_pSurface = reinterpret_cast<ISurface*>(CreateInterface("vguimatsurface.dll", "VGUI_Surface031"));
    if (!g_pSurface.IsValid()) {
        ConsolePrint("Failed to initialize g_pSurface");
        return false;
    }
    ConsolePrint("g_pSurface initialized");

    g_pFontManager = *reinterpret_cast<CFontManager**>(Memory::Scan("vguimatsurface.dll", "74 1D 8B 0D ? ? ? ? 68") + 0x4);
    if (!g_pFontManager.IsValid()) {
        ConsolePrint("Failed to initialize g_pFontManager");
        return false;
    }
    ConsolePrint("g_pFontManager initialized");

    g_pEngine = reinterpret_cast<IVEngineClient*>(CreateInterface("engine.dll", "VEngineClient014"));
    if (!g_pEngine.IsValid()) {
        ConsolePrint("Failed to initialize g_pEngine");
        return false;
    }
    ConsolePrint("g_pEngine initialized");

    g_pEntityList = reinterpret_cast<IClientEntityList*>(CreateInterface("client.dll", "VClientEntityList003"));
    if (!g_pEntityList.IsValid()) {
        ConsolePrint("Failed to initialize g_pEntityList");
        return false;
    }
    ConsolePrint("g_pEntityList initialized");

    return true;
}

void Interfaces::Destroy() {
    g_pChat.Reset();
    g_pEngineVGui.Reset();
    g_pSurface.Reset();
	g_pFontManager.Reset();
	g_pClient.Reset();
	g_pGameEvent.Reset();
	g_pEngine.Reset();
	g_pEntityList.Reset();
	g_pGlobalVars.Reset();
    g_pClientState.Reset();
}