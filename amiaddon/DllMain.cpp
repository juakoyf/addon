#include <Windows.h>
#include "Utils/ConsoleLog.hpp"
#include "SDK/Hooking/Hooks.hpp"
#include "SDK/Interfaces/Interfaces.hpp"
#include "SDK/Classes/CHudChat.hpp"

struct DllArgs {
    HMODULE hModule;
    LPVOID  lpReserved;
};

void Destroy(HMODULE hModule) {

#ifdef DEV 
    ConsoleLog::Destroy();
#endif

    g_Interfaces.Destroy();
    g_Hooks.Destroy();

    FreeLibraryAndExitThread(hModule, 0);
}

DWORD WINAPI Entry(LPVOID param) {
    DllArgs* args = static_cast<DllArgs*>(param);
    if (!args) return 0;

    const HMODULE self = args->hModule;

#ifdef DEV 
    ConsoleLog::Initialize();
#endif

    auto dwProcessPriority = GetPriorityClass(GetCurrentProcess());
    if (dwProcessPriority != REALTIME_PRIORITY_CLASS && dwProcessPriority != HIGH_PRIORITY_CLASS)
        SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

    if (!g_Interfaces.Create(args->lpReserved)) {
        delete args;
        FreeLibraryAndExitThread(self, 0);
        return 0;
    }
    ConsolePrint("Interfaces initialized");

    if (!g_Hooks.Create(args->lpReserved)) {
        delete args;
        FreeLibraryAndExitThread(self, 0);
        return 0;
    }
    ConsolePrint("Hooks initialized");

    ConsolePrint("Injected");

    while (!GetAsyncKeyState(VK_END))
    {
        Sleep(100);
    }

    Destroy(args->hModule);

    delete args;

    FreeLibraryAndExitThread(self, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    if (reason == DLL_PROCESS_ATTACH) {

        DisableThreadLibraryCalls(hModule);

        auto* args = new DllArgs{ hModule, lpReserved };
        if (!args) return FALSE;

        HANDLE hThread = CreateThread(nullptr, 0, Entry, args, 0, nullptr);
        if (hThread) {
            CloseHandle(hThread);
            return TRUE;
        }

        delete args;
        return FALSE;
    }

    return TRUE;
}