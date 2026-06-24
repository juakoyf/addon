#pragma once
#include <cstdint> 
#include "../Utils/VCall.hpp"
#include "../Displacement/Displacement.hpp"
#include "Recv.hpp"

class CCSGOPlayerAnimState;

#define SDK_concat(x, y) x##y
#define SDK_concatiate(x, y) SDK_concat(x, y)
#define SDK_pad(size)                                         \
private:                                                      \
  std::uint8_t SDK_concatiate(__pad, __COUNTER__)[size] = {}; \
public:


class CBaseHandle;
class C_BaseEntity;

class IHandleEntity {
public:
	void SetRefEHandle(const CBaseHandle& handle);
	const CBaseHandle& GetRefEHandle() const;
	const uint32_t& GetRefEHandleRaw() const;
};
class IClientUnknown : public IHandleEntity {
public:
	C_BaseEntity* GetBaseEntity();
};
class IClientEntity : public IClientUnknown {
public:
	SDK_pad(0x64);
    int m_entIndex;
	int EntIndex();
};
class C_BaseEntity : public IClientEntity {
public:
	std::uint8_t& m_MoveType();
	int& m_iTeamNum();
};
class C_BaseAnimating : public C_BaseEntity {};
class C_BaseCombatCharacter : public C_BaseAnimating {
};
class C_BasePlayer : public C_BaseCombatCharacter 
{
public:
	char& m_lifeState();
	int& m_iHealth();
	bool IsDead();
	Vector& m_vecVelocity();
	int& m_fFlags();
};


class C_CSPlayer : public C_BasePlayer {
public:
	static C_CSPlayer* GetLocalPlayer();
	float& m_flLowerBodyYawTarget();
	CCSGOPlayerAnimState*& m_PlayerAnimState();
};

__forceinline C_CSPlayer* ToCSPlayer(C_BaseEntity* pEnt) {
	if (!pEnt || !pEnt->m_entIndex)
		return nullptr;

	return reinterpret_cast<C_CSPlayer*>(pEnt);
}