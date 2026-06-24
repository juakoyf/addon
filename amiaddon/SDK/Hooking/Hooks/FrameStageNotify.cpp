#include "../Hooks.hpp"
#include "../Family/Family.hpp"
#include "../SDK/Interfaces/Interfaces.hpp"

void __fastcall Hooked::hkFrameStageNotify(void* ecx, void* edx, ClientFrameStage_t stage) {

    static const auto ppGameRulesProxy = *reinterpret_cast<CCSGameRules***>(Memory::Scan("client.dll", "8B 0D ? ? ? ? 85 C0 74 0A 8B 01 FF 50 78 83 C0 54") + 2);
    if (*ppGameRulesProxy) {
        g_pGameRules = *ppGameRulesProxy;
    }

    if (stage == FRAME_RENDER_START) {
        static uint32_t m_ClantagChanged = Memory::Scan("engine.dll", "53 56 57 8B DA 8B F9 FF 15");
        static auto fnClantagChanged = (int(__fastcall*)(const char*, const char*))m_ClantagChanged;
        static bool bSetClantag = false;
        if (Family::bClantag) {
            static int nPrevCurtime = (int)g_pGlobalVars->curtime;

            if (nPrevCurtime != (int)g_pGlobalVars->curtime) {
                fnClantagChanged("kaaba.su", "kaaba.su");
                nPrevCurtime = (int)g_pGlobalVars->curtime;
            }

            bSetClantag = true;
        }
        else {
            if (bSetClantag) {
                fnClantagChanged("", "");
                bSetClantag = false;
            }
        }
    }

    oFrameStageNotify(ecx, stage);

}


	