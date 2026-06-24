#pragma once
#include "Vector.hpp"
#include "Player.hpp"

struct aimmatrix_transition_t {
	float	m_flDurationStateHasBeenValid;
	float	m_flDurationStateHasBeenInValid;
	float	m_flHowLongToWaitUntilTransitionCanBlendIn;
	float	m_flHowLongToWaitUntilTransitionCanBlendOut;
	float	m_flBlendValue;

	void Init(void) {
		m_flDurationStateHasBeenValid = 0;
		m_flDurationStateHasBeenInValid = 0;
		m_flHowLongToWaitUntilTransitionCanBlendIn = 0.3f;
		m_flHowLongToWaitUntilTransitionCanBlendOut = 0.3f;
		m_flBlendValue = 0;
	}

	aimmatrix_transition_t() {
		Init();
	}
};

typedef const int* animlayerpreset;

// class created thanks to cs:go source leak :-)
class CCSGOPlayerAnimState {
public:
	animlayerpreset			m_pLayerOrderPreset;

	bool					m_bFirstRunSinceInit;

	bool					m_bFirstFootPlantSinceInit;
	int						m_iLastUpdateFrame;

	float					m_flEyePositionSmoothLerp;

	float					m_flStrafeChangeWeightSmoothFalloff;

	aimmatrix_transition_t	m_tStandWalkAim;
	aimmatrix_transition_t	m_tStandRunAim;
	aimmatrix_transition_t	m_tCrouchWalkAim;

	int						m_cachedModelIndex;

	float					m_flStepHeightLeft;
	float					m_flStepHeightRight;

	void* m_pWeaponLastBoneSetup;

	C_CSPlayer* m_pPlayer;
	void* m_pWeapon;
	void* m_pWeaponLast;

	float					m_flLastUpdateTime;
	int						m_nLastUpdateFrame;
	float					m_flLastUpdateIncrement;

	float					m_flEyeYaw;
	float					m_flEyePitch;
	float					m_flFootYaw;
	float					m_flFootYawLast;
	float					m_flMoveYaw;
	float					m_flMoveYawIdeal;
	float					m_flMoveYawCurrentToIdeal;
	float					m_flTimeToAlignLowerBody;

};//Size=0x344