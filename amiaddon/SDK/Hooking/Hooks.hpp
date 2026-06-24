#pragma once
#include <cstdint>
#include <optional>
#include <SafetyHook/SafetyHook.hpp>
#include "../../Family/Family.hpp"
#include "../../Utils/VCall.hpp"
#include "../../Utils/PatternScan.hpp"
#include "../../Utils/ConsoleLog.hpp"
#include "../Classes/IEngineVGui.hpp"
#include "../Classes/CClientState.hpp"
#include <d3d9.h>

class Hooks {
public:
    bool Create(void* lpReserved);
    void Destroy();
};

extern Hooks g_Hooks;

class Hooking {
    std::optional<safetyhook::InlineHook> m_hook;

public:
    Hooking() = default;
    ~Hooking() { Unhook(); }

    Hooking(const Hooking&) = delete;
    Hooking& operator=(const Hooking&) = delete;

    bool Hook(void* target, void* detour) {
        Unhook();

        if (!target || !detour)
            return false;

        auto hook = safetyhook::create_inline(target, detour, safetyhook::InlineHook::Default);

        if (!hook)
            return false;

        m_hook.emplace(std::move(hook));
        return true;
    }

    bool Hook(uintptr_t target, void* detour) {
        return Hook(reinterpret_cast<void*>(target), detour);
    }

    void Unhook() {
        if (m_hook && *m_hook)
            m_hook->reset();

        m_hook.reset();
    }

    template <typename OrigFn, typename DetourFn>
    OrigFn HookVirtual(void* instance, DetourFn detour, int index) {
        Unhook();
        if (!instance || !detour)
            return nullptr;

        void* target = Memory::VCall<void*>(instance, static_cast<unsigned>(index));

        if (!target)
            return nullptr;

        if (!Hook(target, reinterpret_cast<void*>(detour)))
            return nullptr;

        return GetOriginal<OrigFn>();
    }

    template <typename Fn>
    Fn GetOriginal() const {
        if (!m_hook || !*m_hook)
            return nullptr;

        return m_hook->original<Fn>();
    }

    bool IsHooked() const noexcept {
        return m_hook.has_value();
    }
};

#include "../Classes/SDK.hpp"
#include "../Classes/Player.hpp"
#include "../Classes/CUserCmd.hpp"
#include "../Interfaces/Interfaces.hpp"

class CUserCmd;

namespace Hooked {

    typedef void(__thiscall* PaintFn)(IEngineVGui*, int);
    inline Hooking PaintHook;
    inline PaintFn oPaint;
    void __stdcall hkPaint(int mode);

    using ResetFn = HRESULT(__stdcall*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
    inline Hooking ResetHook;
    inline ResetFn oReset;
    HRESULT __stdcall hkReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);

    using FrameStageNotifyFn = void(__thiscall*)(void*, ClientFrameStage_t);
    inline Hooking FrameStageNotifyHook;
    inline FrameStageNotifyFn oFrameStageNotify;
    void __fastcall hkFrameStageNotify(void* ecx, void* edx, ClientFrameStage_t stage);

    using StartMoveFn = void(__fastcall*)(void* ecx, void* edx, CUserCmd* cmd);
    inline Hooking StartMoveHook;
    inline StartMoveFn oStartMove;
    void __fastcall hkStartMove(void* ecx, void* edx, CUserCmd* cmd);

    using AntiAimFn = char* (__fastcall*)(int32_t* g_hvh);
    inline Hooking AntiAimHook;
    inline AntiAimFn oAntiAim;
    char* __fastcall hkAntiAim(int32_t* g_hvh);

    // AntiAim
    inline CUserCmd* pCurrentCmd;

}