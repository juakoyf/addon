#include "../Hooks.hpp"

void __fastcall Hooked::hkStartMove(void* ecx, void* edx, CUserCmd* cmd) {
	Hooked::pCurrentCmd = cmd;
	return oStartMove(ecx, edx, cmd);
}