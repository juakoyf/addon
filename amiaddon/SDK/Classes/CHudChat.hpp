#pragma once
#include "../../Utils/VCall.hpp"
#include <cstdio>
#include <cstdarg>

class CBaseHudChat
{
public:
    void ChatPrintf(int iPlayerIndex, int iFilter, const char* fmt, ...)
    {
        char buf[4096];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);

        // char __thiscall sub_104287B0(int *this, int a2)
        // vtable[26]
        using Fn = void(__cdecl*)(void*, int, int, const char*, ...);
        auto fn = Memory::VCall<Fn>(this, 26);
        if (fn)
            fn(this, iPlayerIndex, iFilter, "%s", buf);
    }
};
