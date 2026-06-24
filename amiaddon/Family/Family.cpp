#include "Family.hpp"
#include <windows.h>
#include <tlhelp32.h>
#include "../Utils/ConsoleLog.hpp"
#include "../SDK/Interfaces/Interfaces.hpp"

struct MOString {
    char buffer[16];
    size_t len;
    size_t capacity;

    MOString(const char* str) {
        memset(this, 0, sizeof(MOString));
        len = strlen(str);
        if (len > 15) len = 15;
        memcpy(buffer, str, len);
        capacity = 15;
    }
};

bool Family::InitializeFamily() {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    if (hSnap == INVALID_HANDLE_VALUE) return false;

    MODULEENTRY32 modEntry;
    modEntry.dwSize = sizeof(modEntry);

    if (Module32First(hSnap, &modEntry)) {
        do {
            if (modEntry.hModule) {
                PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)modEntry.hModule;
                if (dos->e_magic == IMAGE_DOS_SIGNATURE) {
                    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)((uintptr_t)modEntry.hModule + dos->e_lfanew);
                    if (nt->Signature == IMAGE_NT_SIGNATURE) {
                        if (nt->OptionalHeader.AddressOfEntryPoint == 0xD3354) {
                            dwFamilyBase = (uintptr_t)modEntry.hModule;
                            CloseHandle(hSnap);
                            ConsolePrint("Found Family Module: %s Base: 0x%X", modEntry.szModule, dwFamilyBase);
                            return true;
                        }
                    }
                }
            }
        } while (Module32Next(hSnap, &modEntry));
    }

    CloseHandle(hSnap);
    return false;
}

int Family::GetWeaponID() {
    if (!Family::dwFamilyBase) return -1;
    uintptr_t addr = Family::dwFamilyBase + offsetWeaponID;
    if (IsBadReadPtr((void*)addr, sizeof(int))) return -1;
    return *(int*)addr;
}

void Family::PrintChat(const char* configName) {
    if (g_pChat.IsValid()) {
        char buf[256];
        sprintf_s(buf, "[\x0A" "ami\x0Bhack\x01] Loaded \x06%s\x01 config", configName);
        g_pChat->ChatPrintf(0, 0, buf);
    }
}

void Family::FilterTabs(void* pFlag) {
    if (!pFlag) return;

    uintptr_t* pBegin = *(uintptr_t**)((uintptr_t)pFlag + 0x28);
    uintptr_t* pEnd   = *(uintptr_t**)((uintptr_t)pFlag + 0x2C);

    if (!pBegin || !pEnd) return;

    size_t count = (pEnd - pBegin);
    for (size_t i = 0; i < count; i++) {
        uintptr_t pTab = pBegin[i];
        if (!pTab) continue;

        const char* name = (const char*)pTab;
        if (IsBadReadPtr(name, 32)) continue;

        bool* pIgnore = (bool*)(pTab + 0x7C); 

        if (strstr(name, "aimbot") != nullptr) {
            *pIgnore = false;
        } else {
            *pIgnore = true;
        }
    }
}

typedef void(__thiscall* FnLoadConfig)(void* pManager, void* pFlag, MOString* pStr);

void Family::LoadConfig(const char* configName) {
    if (!Family::dwFamilyBase) return;

    FnLoadConfig fn = (FnLoadConfig)(Family::dwFamilyBase + 0x5D970);
    void* pManager = (void*)(Family::dwFamilyBase + 0x13FF78);
    void* pFlag    = (void*)(Family::dwFamilyBase + 0x141ED0);

    Family::FilterTabs(pFlag);

    MOString str(configName);
    fn(pManager, pFlag, &str);
}

void Family::PatchDropdown() {
    if (!Family::dwFamilyBase) return;

    void* pFlag = (void*)(Family::dwFamilyBase + 0x141ED0);
    if (!pFlag || IsBadReadPtr(pFlag, 4)) return;
    
    uintptr_t* pBegin = *(uintptr_t**)((uintptr_t)pFlag + 0x28);
    if (!pBegin || IsBadReadPtr(pBegin, 32)) return;
    
    uintptr_t pTab7 = pBegin[7];
    if (!pTab7 || IsBadReadPtr((void*)pTab7, 64)) return;

    uintptr_t start = *(uintptr_t*)(pTab7 + 0x1C);
    uintptr_t end   = *(uintptr_t*)(pTab7 + 0x20);
    
    if (!start || !end || end <= start || (end - start) > 0x2000) return;

    size_t count = (end - start) / 4;
    uintptr_t* pItems = (uintptr_t*)start;

    static const char* mapNames[][2] = {
        {"1", "scout"}, {"2", "auto"}, {"3", "awp"},
        {"4", "pistols"}, {"5", "heavy pistols"}, {"6", "default"}
    };

    for (size_t k = 0; k < count; k++) {
        uintptr_t pItem = pItems[k];
        if (!pItem || IsBadReadPtr((void*)pItem, 300)) continue;
        for (int offset = 0; offset < 256; offset += 4) {
             uintptr_t vStart = *(uintptr_t*)(pItem + offset);
             uintptr_t vEnd   = *(uintptr_t*)(pItem + offset + 4);
             uintptr_t vCap   = *(uintptr_t*)(pItem + offset + 8);

             if (vStart && vEnd && vEnd > vStart && vCap >= vEnd && (vEnd - vStart) < 0x800) {
                 size_t bytes = vEnd - vStart;
                 
                 if (bytes % 24 == 0) {
                     size_t num = bytes / 24;
                     if (num >= 5 && num <= 30) {
                         uintptr_t sAddr = vStart;
                         if (!IsBadReadPtr((void*)sAddr, 24)) {
                             size_t len = *(size_t*)(sAddr + 16);
                             size_t cap = *(size_t*)(sAddr + 20);
                             if (len == 1 && cap >= 15 && cap < 200) {
                                 char* buf = (char*)sAddr;
                                 if (!IsBadReadPtr(buf, 4) && buf[0] == '1' && buf[1] == 0) {
                                     for (size_t i = 0; i < num; i++) {
                                         uintptr_t elem = vStart + i * 24;
                                         if (IsBadReadPtr((void*)elem, 24)) continue;
                                         char* eBuf = (char*)elem;
                                         size_t eCap = *(size_t*)(elem + 20);
                                         
                                         if (eCap >= 15 && !IsBadReadPtr(eBuf, 16)) {
                                             char key = eBuf[0];
                                             for (auto& pair : mapNames) {
                                                 if (pair[0][0] == key) {
                                                      strcpy_s(eBuf, 16, pair[1]);
                                                      *(size_t*)(elem + 16) = strlen(pair[1]);
                                                      break;
                                                 }
                                             }
                                         }
                                     }
                                 }
                             }
                         }
                     }
                 }
             }
        }
    }
}

void Family::CreateMultiDropdown() {
    if (!Family::dwFamilyBase) return;

    struct Dummy { };
    
    auto createStr = [](void* buf, const char* str) {
        typedef void (__thiscall Dummy::*FnConstructor)(const char*);
        union { uintptr_t addr; FnConstructor fn; } u;
        u.addr = Family::dwFamilyBase + 0xE610;
        ((Dummy*)buf->*u.fn)(str);
    };

    auto destroyStr = [](void* buf) {
        typedef void (__thiscall Dummy::*FnDestructor)();
        union { uintptr_t addr; FnDestructor fn; } u;
        u.addr = Family::dwFamilyBase + 0xF2F0;
        ((Dummy*)buf->*u.fn)();
    };

    void* pFlag = (void*)(Family::dwFamilyBase + 0x141ED0);
    if (!pFlag || IsBadReadPtr(pFlag, 4)) return;

    uintptr_t* pBegin = *(uintptr_t**)((uintptr_t)pFlag + 0x28);
    if (!pBegin || IsBadReadPtr(pBegin, 32)) return;

    void* pTabConfig = (void*)pBegin[7]; 
    if (!pTabConfig || IsBadReadPtr(pTabConfig, 64)) return;
    
    void* pTabVisuals = (void*)pBegin[3];
    if (!pTabVisuals || IsBadReadPtr(pTabVisuals, 64)) return;

    alignas(16) static char pMultiDropdownInstance[2048]; 
    static int finalFlagOffset = -1;

    if (pMultiDropdownInstance[0] == 0) { 
        memset(pMultiDropdownInstance, 0, sizeof(pMultiDropdownInstance));

        uintptr_t childrenStart = *(uintptr_t*)((uintptr_t)pTabVisuals + 0x1C);
        uintptr_t childrenEnd   = *(uintptr_t*)((uintptr_t)pTabVisuals + 0x20);
        
        bool bCloneFound = false;

        if (childrenStart && childrenEnd && childrenEnd > childrenStart) {
            size_t count = (childrenEnd - childrenStart) / 4;
            uintptr_t* pItems = (uintptr_t*)childrenStart;

            for (size_t k = 0; k < count; k++) {
                uintptr_t pItem = pItems[k];
                if (!pItem || IsBadReadPtr((void*)pItem, 300)) continue;

                bool isTargetDropdown = false;
                int vectorOffset = 0;
                
                for (int offset = 0; offset < 256; offset += 4) {
                        uintptr_t vStart = *(uintptr_t*)(pItem + offset);
                        uintptr_t vEnd   = *(uintptr_t*)(pItem + offset + 4);
                        uintptr_t vCap   = *(uintptr_t*)(pItem + offset + 8);

                        if (vStart && vEnd && vEnd > vStart && vCap >= vEnd && (vEnd - vStart) < 0x800) {
                            size_t bytes = vEnd - vStart;
                            if (bytes == 96) {
                                isTargetDropdown = true;
                                vectorOffset = offset;
                                break;
                            }
                        }
                }

                if (isTargetDropdown) {
                    memcpy(pMultiDropdownInstance, (void*)pItem, 512);
                    
                    static char myItemsBuffer[96]; 
                    memset(myItemsBuffer, 0, 96);

                    const char* newOpts[] = { "automatic load", "watermark", "killsay", "clantag" };
                    for(int i=0; i<4; i++) {
                        uintptr_t pStr = (uintptr_t)myItemsBuffer + i * 24;
                        createStr((void*)pStr, newOpts[i]);
                    }
                    
                    if (vectorOffset > 0) {
                        uintptr_t* pVec = (uintptr_t*)(pMultiDropdownInstance + vectorOffset);
                        pVec[0] = (uintptr_t)myItemsBuffer;
                        pVec[1] = (uintptr_t)myItemsBuffer + 96; 
                        pVec[2] = (uintptr_t)myItemsBuffer + 96; 
                        ConsolePrint("Deep Copied Vector! Detached from original.");
                    }

                    int flagOffset = -1;
                    uintptr_t ptrVTable = *(uintptr_t*)pItem;

                    for (int off = 4; off < 1024; off += 4) {
                        uintptr_t val = *(uintptr_t*)(pItem + off);
                        
                        if (val == 0 || val == ptrVTable) continue;
                        if (val >= (uintptr_t)pItem && val < ((uintptr_t)pItem + 2048)) continue; 
                        if (off >= vectorOffset && off <= vectorOffset + 12) continue;

                        if (!IsBadReadPtr((void*)val, 4)) {
                             int flagVal = *(int*)val;
                             if (flagVal >= 0 && flagVal <= 100) { 

                                  ConsolePrint("Candidate Flag at Offset 0x%X (Val: %d, Ptr: 0x%X)", off, flagVal, val);
                                  
                                  if (flagOffset == -1) {
                                      flagOffset = off; 
                                  }
                             }
                        }
                    }

                    if (flagOffset != -1) {
                         ConsolePrint("Found Flag Pointer at offset 0x%X (will patch after CreateObject)", flagOffset);
                    } else {
                         ConsolePrint("Error: Could not find Flag Pointer offset in relaxed scan!");
                    }

                    static int finalFlagOffset = flagOffset;
                    if (flagOffset != -1) finalFlagOffset = flagOffset;

                    ConsolePrint("Cloned 'World' Dropdown (4 items) from 0x%X (Tab 3)", pItem);
                    bCloneFound = true;
                    break;
                }
            }
        }
        
        if (!bCloneFound) {
             ConsolePrint("Error: Could not find 'World' Dropdown (4 items) in Tab 3!");
             return; 
        }
    }

    typedef void (__thiscall Dummy::*FnCreateLabel)(void*, int);
    union { uintptr_t addr; FnCreateLabel fn; } uCreateLabel;
    uCreateLabel.addr = Family::dwFamilyBase + 0x456A0;

    ((Dummy*)pTabConfig->*uCreateLabel.fn)(pMultiDropdownInstance, 0);

    struct MyString { char buf[24]; }; 
    static_assert(sizeof(MyString) == 24, "MyString size mismatch");

    MyString options[4];
    createStr(options[0].buf, "automatic load");      
    createStr(options[1].buf, "watermark");
    createStr(options[2].buf, "killsay");
    createStr(options[3].buf, "clantag");

    struct MyVector {
        MyString* _Myfirst;
        MyString* _Mylast;
        MyString* _Myend;
    } vecOptions;

    vecOptions._Myfirst = options;
    vecOptions._Mylast = options + 4;
    vecOptions._Myend = options + 4; 

    static int myFlags = 0;
    Family::pMultiDropdownFlags = &myFlags;
    
    static char vectorBuffer[64]; 
    memset(vectorBuffer, 0, 64);

    struct MyVectorEmpty { void* a=0; void* b=0; void* c=0; } vecEmpty;
    
    typedef void (__thiscall Dummy::*FnPopulate)(void*, void*, void*);
    union { uintptr_t addr; FnPopulate fn; } uPopulate;
    uPopulate.addr = Family::dwFamilyBase + 0x43790;
    
    ((Dummy*)vectorBuffer->*uPopulate.fn)(&vecOptions, &vecEmpty, &myFlags);

    char sName[32]; createStr(sName, "addon options"); 
    char sConfigName[32]; createStr(sConfigName, "addon_options"); 

    void* pThis = pMultiDropdownInstance;
    void* pDisplayName = sName;
    void* pConfigKey = sConfigName;
    void* pData = vectorBuffer;
    uintptr_t fnAddress = Family::dwFamilyBase + 0x83910;

    __asm {
        mov ecx, pThis
        push 0 // arg_1C
        push 0 // arg_18
        push 0 // arg_14
        push 1 // flag (arg_10)
        push pData      // arg_C (Vector Data)
        push pConfigKey // arg_8 (Config Key)
        push pDisplayName // arg_4 (Display Name)
        call fnAddress
    }

    typedef void (__thiscall Dummy::*FnRegister)();
    union { uintptr_t addr; FnRegister fn; } uRegister;
    uRegister.addr = Family::dwFamilyBase + 0x46A10;
    
    ((Dummy*)vectorBuffer->*uRegister.fn)();

    Family::pWorldDropdown = pMultiDropdownInstance; 
    
    destroyStr(sName);
    destroyStr(sConfigName);
    for(int i=0; i<4; i++) destroyStr(options[i].buf);
    
    ConsolePrint("Internal MultiDropdown Copied from Tab 3 & Registered to Tab 7!");
}

void Family::OnPaint() {
    if (!Family::dwFamilyBase) return;

    if (!Family::bInitialized) {
        Family::bInitialized = true;

        Family::PatchDropdown();
        Family::CreateMultiDropdown();
    }

    // Update FamilyCurTime every frame (was previously inside bInitialized block, only ran once)
    uintptr_t* ppGlobalVars = (uintptr_t*)(Family::dwFamilyBase + Family::offsetGlobalVars);

    if (ppGlobalVars && !IsBadReadPtr(ppGlobalVars, sizeof(uintptr_t))) {
        CGlobalVars* pFamGlobals = (CGlobalVars*)*ppGlobalVars;

        if (pFamGlobals && !IsBadReadPtr(pFamGlobals, sizeof(CGlobalVars))) {
            Family::FamilyCurTime = pFamGlobals->curtime;
        }
    }

    static void* pInstance = nullptr;
    if (!pInstance) {
        pInstance = Family::pWorldDropdown;
    }

    if (pInstance) {
        uintptr_t _Myfirst = *(uintptr_t*)((uintptr_t)pInstance + 0x110);
        uintptr_t _Mylast  = *(uintptr_t*)((uintptr_t)pInstance + 0x114);

        bool newAutoLoad = false;
        bool newWatermark = false;
        bool newKillsay = false;
        bool newClantag = false;

        if (_Myfirst && _Mylast && _Mylast >= _Myfirst) {
             size_t count = (_Mylast - _Myfirst) / 4;
             if (count < 100) {
                 int* pIndices = (int*)_Myfirst;
                 for (size_t i = 0; i < count; i++) {
                     int idx = pIndices[i];
                     if (idx == 0) newAutoLoad = true;
                     if (idx == 1) newWatermark = true;
                     if (idx == 2) newKillsay = true;
                     if (idx == 3) newClantag = true;
                 }
             }
        }

        Family::bAutoLoad = newAutoLoad;
        Family::bWatermark = newWatermark;
        Family::bKillsay = newKillsay;
        Family::bClantag = newClantag;

        static int lastHash = -1;
        int curHash = (newAutoLoad) | (newWatermark << 1) | (newKillsay << 2) | (newClantag << 3);
        if (curHash != lastHash) {
             ConsolePrint("Flags Updated: Auto=%d, Water=%d, Killsay=%d, Clantag=%d", 
                 newAutoLoad, newWatermark, newKillsay, newClantag);
             lastHash = curHash;
        }
    }

    if (Family::bAutoLoad) {
        static int lastLoadedConfigIndex = -1;
        int currentWeaponID = Family::GetWeaponID();

        if (currentWeaponID != -1 && currentWeaponID != Family::iLastWeaponID) {

            bool bIsIgnoredWeapon = false;
            if (currentWeaponID == 42 || currentWeaponID == 59 || currentWeaponID >= 500) bIsIgnoredWeapon = true;
            if (currentWeaponID >= 43 && currentWeaponID <= 49) bIsIgnoredWeapon = true;

            if (!bIsIgnoredWeapon) {
                int targetConfigIndex = -1;
                const char* cfgName = "";

                switch (currentWeaponID) {
                case 40: targetConfigIndex = 0; cfgName = "scout"; break;
                case 11: case 38: targetConfigIndex = 1; cfgName = "auto"; break;
                case 9: targetConfigIndex = 2; cfgName = "awp"; break;
                case 64: case 1: targetConfigIndex = 4; cfgName = "heavy pistols"; break;
                case 2: case 3: case 4: case 30: case 32: case 36: case 61: case 63: targetConfigIndex = 3; cfgName = "pistols"; break;
                default: targetConfigIndex = 5; cfgName = "default"; break;
                }

                if (targetConfigIndex != -1 && targetConfigIndex != lastLoadedConfigIndex) {
                    Family::LoadConfig(cfgName);
                    Family::PrintChat(cfgName);
                    lastLoadedConfigIndex = targetConfigIndex;
                }
            }
        }
        Family::iLastWeaponID = currentWeaponID;
    }
}