#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>

namespace ConsoleLog {

    inline void Initialize()
    {
        AllocConsole();
        SetConsoleTitleA("ami");
        freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
        freopen_s(reinterpret_cast<FILE**>(stderr), "CONOUT$", "w", stderr);
        freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);

        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = 0;
        GetConsoleMode(hOut, &mode);
        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, mode);
    }

    inline void Destroy()
    {
        fflush(stdout);
        fflush(stderr);
        fflush(stdin);

        fclose(stdout);
        fclose(stderr);
        fclose(stdin);

        HWND consoleWnd = GetConsoleWindow();
        if (consoleWnd) {
            ShowWindow(consoleWnd, SW_HIDE);
        }

        FreeConsole();

        if (consoleWnd && IsWindow(consoleWnd)) {
            PostMessageA(consoleWnd, WM_CLOSE, 0, 0);
        }
    }
};

inline void ConsolePrint(const char* fmt, ...)
{
    const int R = 185;
    const int G = 85;
    const int B = 115;

    printf("\x1b[38;2;%d;%d;%dmami \x1b[0m", R, G, B);

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    printf("\n");
}