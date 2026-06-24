#pragma once
#include <Windows.h>
#include <string>
#include "../../Utils/ConsoleLog.hpp"
#include "../Classes/IEngineVGui.hpp"
#include "../Classes/ISurface.hpp"
#include "../Classes/CHudChat.hpp"
#include "../Classes/IBaseClientDll.hpp"
#include "../Classes/IGameEventManager.hpp"
#include "../Classes/CFontManager.hpp"
#include "../Classes/IVEngineClient.hpp"
#include "../Classes/IClientEntityList.hpp"
#include "../Classes/CGlobalVars.hpp"
#include "../Classes/CClientState.hpp"
#include "../Classes/CCSGameRules.hpp"

template <typename T>
class Ptr {
    T* m_ptr = nullptr;

public:
    Ptr() = default;
    Ptr(T* ptr) : m_ptr(ptr) {}

    Ptr& operator=(T* ptr) {
        m_ptr = ptr;
        return *this;
    }

    T* Get() const {
        return m_ptr;
    }

    T* operator->() const {
        return m_ptr;
    }

    bool IsValid() const {
        return m_ptr != nullptr;
    }

    explicit operator bool() const {
        return IsValid();
    }

    void Reset() noexcept {
        m_ptr = nullptr;
    }
};

class CBaseHudChat;
class IEngineVGui;
class ISurface;
class IBaseClientDLL;
class IGameEventManager;
class CFontManager;
class IVEngineClient;
class IClientEntityList;
class CGlobalVars;
class CCSGameRules;

extern Ptr<IBaseClientDLL> g_pClient;
extern Ptr<CBaseHudChat> g_pChat;
extern Ptr<IEngineVGui> g_pEngineVGui;
extern Ptr<ISurface> g_pSurface;
extern Ptr<IGameEventManager> g_pGameEvent;
extern Ptr<CFontManager> g_pFontManager;
extern Ptr<IVEngineClient> g_pEngine;
extern Ptr<IClientEntityList> g_pEntityList;
extern Ptr<CGlobalVars> g_pGlobalVars;
extern Ptr<CClientState> g_pClientState;
extern Ptr<CCSGameRules> g_pGameRules;

class Interfaces {
public:
    bool Create(void* lpReserved);
    void Destroy();

    using CreateInterfaceFn = void* (__cdecl*)(const char*, int*);

    void* CreateInterface(const std::string& moduleName, const std::string& interfaceName) {
        HMODULE hModule = GetModuleHandleA(moduleName.c_str());
        if (!hModule)
            return nullptr;

        auto fnCreateInterface = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(hModule, "CreateInterface"));

        if (!fnCreateInterface)
            return nullptr;

        return fnCreateInterface(interfaceName.c_str(), nullptr);
    }

    HWND hWindow = nullptr;
};

extern Interfaces g_Interfaces;