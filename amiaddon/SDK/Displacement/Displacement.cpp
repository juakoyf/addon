#include "Displacement.hpp"
#include "../Utils/PatternScan.hpp"
namespace Engine
{
	DllInitializeData Displacement;

	void Create() {

		Displacement.DT_BasePlayer.m_lifeState = g_PropManager.GetOffset(("DT_BasePlayer"), ("m_lifeState"));
		Displacement.DT_BasePlayer.m_iHealth = g_PropManager.GetOffset(("DT_BasePlayer"), ("m_iHealth"));
		Displacement.DT_CSPlayer.m_flLowerBodyYawTarget = g_PropManager.GetOffset("DT_CSPlayer", "m_flLowerBodyYawTarget");
		Displacement.DT_BasePlayer.m_vecVelocity = g_PropManager.GetOffset("DT_BasePlayer", "m_vecVelocity[0]");
		Displacement.C_CSPlayer.m_PlayerAnimState = *(int*)(Memory::Scan("client.dll", "8B 8E ? ? ? ? 85 C9 74 3E") + 2);
	}


	bool CreateDisplacement(void* lpReserved) {
		Create();

		return true;
	}
}