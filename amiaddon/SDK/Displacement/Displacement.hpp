#pragma once
#include <cstdint>
#include "PropManager.hpp"

struct DllInitializeData {

	struct {
	} Data;

	// DT_CSPlayer 
	struct {
		uint32_t m_flLowerBodyYawTarget;
	} DT_CSPlayer;

	// DT_BaseAttributableItem
	struct {
		
	} DT_BaseAttributableItem;

	// DT_BaseEntity
	struct {
	
	} DT_BaseEntity;

	// DT_BaseAnimating
	struct {
		
	} DT_BaseAnimating;

	// C_BaseEntity
	struct {
		
	} C_BaseEntity;

	// DT_BasePlayer
	struct {
		uint32_t m_lifeState;
		uint32_t m_iHealth;
		uint32_t m_vecVelocity;
	} DT_BasePlayer;

	// C_CSPlayer
	struct {
		uint32_t m_PlayerAnimState;
	} C_CSPlayer;

	// DT_BaseViewModel
	struct {

	} DT_BaseViewModel;

	// DT_BaseCombatCharacter
	struct {

	} DT_BaseCombatCharacter;


	struct {

	} DT_BaseCombatWeapon;

	// C_BaseAnimating
	struct {
	} C_BaseAnimating;

	struct {
		uint32_t m_uGetSequenceActivity;
	} Function;
};
namespace Engine
{
	extern DllInitializeData Displacement;

	bool CreateDisplacement(void* lpReserved);
}