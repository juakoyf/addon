#include "Hooks.hpp"
#include "../Family/Family.hpp"
#include "../Interfaces/Interfaces.hpp"
#include "../Utils/GameEvent.hpp"

Hooks g_Hooks;

bool Hooks::Create(void* lpReserved) {

	if (!Family::InitializeFamily()) {
        ConsolePrint("Failed to initialize Family module");
        MessageBoxA(NULL, "Family not loaded. Inject family before the addon", "Error", MB_OK | MB_ICONERROR);
        return false;
    }

    Hooked::oPaint = Hooked::PaintHook.HookVirtual<Hooked::PaintFn>(g_pEngineVGui.Get(), &Hooked::hkPaint, 14);
    if (!Hooked::PaintHook.IsHooked() || !Hooked::oPaint) {
        ConsolePrint("Failed to hook Paint");
        return false;
    }
    ConsolePrint("Paint hooked successfully");

    Hooked::oFrameStageNotify = Hooked::FrameStageNotifyHook.HookVirtual<Hooked::FrameStageNotifyFn>(g_pClient.Get(), &Hooked::hkFrameStageNotify, 36);
    if (!Hooked::FrameStageNotifyHook.IsHooked() || !Hooked::oFrameStageNotify) {
        ConsolePrint("Failed to hook FrameStageNotify");
        return false;
    }
    ConsolePrint("FrameStageNotify hooked successfully");

    Hooked::StartMoveHook.Hook(Memory::Scan(Family::dwFamilyBase, "55 8B EC 83 EC ?? 56 8B 75 ?? 57 8B F9 89 7D"), Hooked::hkStartMove);
    Hooked::oStartMove = Hooked::StartMoveHook.GetOriginal<Hooked::StartMoveFn>();
    if (!Hooked::StartMoveHook.IsHooked() || !Hooked::oStartMove) {
        ConsolePrint("Failed to hook StartMove");
        return false;
    }
    ConsolePrint("StartMove hooked successfully");

    Hooked::AntiAimHook.Hook(Memory::Scan(Family::dwFamilyBase, "55 8B EC 83 EC 2C 56 8B F1 B9 96 00 00 00"), Hooked::hkAntiAim);
    Hooked::oAntiAim = Hooked::AntiAimHook.GetOriginal<Hooked::AntiAimFn>();
    if (!Hooked::AntiAimHook.IsHooked() || !Hooked::oAntiAim) {
        ConsolePrint("Failed to hook AntiAim");
        return false;
    }
    ConsolePrint("AntiAim hooked successfully");

    g_GameEvent.Register();

    return true;
}

void Hooks::Destroy() {

    if (Hooked::PaintHook.IsHooked()) {
        Hooked::PaintHook.Unhook();
    }

    if (Hooked::ResetHook.IsHooked()) {
        Hooked::ResetHook.Unhook();
    }

    if (Hooked::FrameStageNotifyHook.IsHooked()) {
        Hooked::FrameStageNotifyHook.Unhook();
    }

	if (Hooked::StartMoveHook.IsHooked()) {
        Hooked::StartMoveHook.Unhook();
    }

	if (Hooked::AntiAimHook.IsHooked()) {
        Hooked::AntiAimHook.Unhook();
    }

    g_GameEvent.Shutdown();
}