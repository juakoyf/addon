#pragma once
#include "Vector.hpp"
#include "QAngle.hpp"

using CRC32_t = unsigned int;

class CUserCmd
{
public:
	inline bool HasFlag(int flag) {
		if (!this)
			return false;

		return this->buttons & flag;
	}

	inline void SetFlag(int flag) {
		if (!this)
			return;

		this->buttons |= flag;
	}

	inline void RemoveFlag(int flag) {
		if (!this)
			return;

		this->buttons &= ~flag;
	}

	char pad_0x0000[0x4]; //0x0000	
	int     command_number;     // 0x04 For matching server and client commands for debugging
	int     tick_count;         // 0x08 the tick the client created this command
	QAngle  viewangles;         // 0x0C Player instantaneous view angles.
	Vector  aimdirection;       // 0x18
	float   forwardmove;        // 0x24
	float   sidemove;           // 0x28
	float   upmove;             // 0x2C
	int     buttons;            // 0x30 Attack button states
	char    impulse;            // 0x34
	int     weaponselect;       // 0x38 Current weapon id
	int     weaponsubtype;      // 0x3C
	int     random_seed;        // 0x40 For shared random functions
	short   mousedx;            // 0x44 mouse accum in x from create move
	short   mousedy;            // 0x46 mouse accum in y from create move
	bool    hasbeenpredicted;   // 0x48 Client only, tracks whether we've predicted this command at least once
	Vector  m_head_angles;      // 0x4C
	Vector  m_head_offset;      // 0x58
public:
	bool send_packet;
};
