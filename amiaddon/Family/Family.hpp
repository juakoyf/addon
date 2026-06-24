#pragma once
#include <cstdint>
#include "../SDK/Classes/CUserCmd.hpp"

namespace Family {
	inline uintptr_t dwFamilyBase = 0;
	inline uintptr_t offsetWeaponID = 0x1CFD70;
	inline uintptr_t offsetGlobalVars = 0x141B78;

	inline float FamilyCurTime = 0.0f;

	inline bool bInitialized = false;
	inline int iLastWeaponID = -1;
	inline bool bAutoLoad = false;
	inline bool bWatermark = false;
	inline bool bKillsay = false;
	inline bool bClantag = false;

	inline bool bGameOver = false;
	inline bool bRoundEnd = false;
	
	inline void* pWorldDropdown = nullptr;
	inline int* pMultiDropdownFlags = nullptr;

	typedef void(__thiscall* FnStringConstructor)(void* thisptr, const char* str);
	typedef void(__thiscall* FnStringDestructor)(void* thisptr);
	typedef void(__thiscall* FnCreateLabel)(void* thisptr, int a2, void* a3); // sub_100456A0
	typedef void(__stdcall* FnPopulateMultiDropdown)(void* vector, int a2, int a3); // sub_10043790
	typedef void(__cdecl* FnCreateDropdownObject)(void* a1, void* a2, void* a3); // sub_10083910
	typedef void(__thiscall* FnRegisterDropdown)(void* thisptr); // sub_10046A10

	bool InitializeFamily();
	int GetWeaponID();
	void PrintChat(const char* configName);
	void FilterTabs(void* pFlag);
	void LoadConfig(const char* configName);
	void PatchDropdown();
	void CreateMultiDropdown();
	void OnPaint();
}
