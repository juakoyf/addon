#include "Player.hpp"
#include "PlayerAnimState.hpp"
#include "../Interfaces/Interfaces.hpp"

void IHandleEntity::SetRefEHandle(const CBaseHandle& handle) {
	using Fn = void(__thiscall*)(void*, const CBaseHandle&);
	return Memory::VCall<Fn>(this, 1)(this, handle);
}

const CBaseHandle& IHandleEntity::GetRefEHandle() const {
	using Fn = const CBaseHandle& (__thiscall*)(const IHandleEntity*);
	return Memory::VCall<Fn>(this, 2)(this);
}

const uint32_t& IHandleEntity::GetRefEHandleRaw() const {
	using Fn = const uint32_t& (__thiscall*)(const IHandleEntity*);
	return Memory::VCall<Fn>(this, 2)(this);
}

C_BaseEntity* IClientUnknown::GetBaseEntity() {
	using Fn = C_BaseEntity * (__thiscall*)(void*);
	return Memory::VCall<Fn>(this, 7)(this);
}

char& C_BasePlayer::m_lifeState() {
	return *(char*)((uintptr_t)this + 0x025B);
}


int& C_BasePlayer::m_iHealth() {
	return *(int*)((uintptr_t)this + 0x00FC);
}

Vector& C_BasePlayer::m_vecVelocity() {
	return *(Vector*)((uintptr_t)this + 0x0110);
}

float& C_CSPlayer::m_flLowerBodyYawTarget() {
	return *(float*)((uintptr_t)this + 0x39DC);
}

CCSGOPlayerAnimState*& C_CSPlayer::m_PlayerAnimState() {
	return *(CCSGOPlayerAnimState**)((uintptr_t)this + 0x3874);
}

bool C_BasePlayer::IsDead() {
	if (!this)
		return false;

	return (this->m_lifeState()) || !this->m_iHealth();
}
C_CSPlayer* C_CSPlayer::GetLocalPlayer() {

	auto index = g_pEngine->GetLocalPlayer();

	if (!index)
		return nullptr;

	auto client = g_pEntityList->GetClientEntity(index);

	if (!client)
		return nullptr;

	return ToCSPlayer(client->GetBaseEntity());
}

int IClientEntity::EntIndex() {
	if (!this || this == nullptr)
		return -1;

	return this->m_entIndex;
}

std::uint8_t& C_BaseEntity::m_MoveType() {
	return *(std::uint8_t*)((uintptr_t)this + 0x0258);
}

int& C_BasePlayer::m_fFlags() {
	return *(int*)((uintptr_t)this + 0x0100);
}

int& C_BaseEntity::m_iTeamNum() {
	return *(int*)((uintptr_t)this + 0x00F0);
}