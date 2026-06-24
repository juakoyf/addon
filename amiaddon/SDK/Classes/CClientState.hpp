#pragma once
#include <cstdint>

class CClientState {
private:
	char pad1[0x9C];                                // 0x0000

public:
	void* m_net_channel;                // 0x009C
private:
	char pad2[0x70];                                // 0x00A0

public:
	int                m_next_message_time;        // 0x0110

public:
	float           m_net_cmd_time;             // 0x0114
	uint32_t        m_server_count;             // 0x0118
private:
	char pad3[0x4C];                                // 0x011C

public:
	int             m_unk;                      // 0x0168
	int             m_server_tick;              // 0x016C
	int             m_client_tick;              // 0x0170
	int             m_delta_tick;               // 0x0174
	bool            m_paused;                   // 0x0178

private:
	char pad4[0x4B2F];                              // 0x0179

public:
	float           m_frame_time;               // 0x4CA8
	int             m_last_outgoing_command;    // 0x4CAC
	int             m_choked_commands;          // 0x4CB0
	int             m_last_command_ack;         // 0x4CB4
	uint32_t        m_nSoundSequence;
	int             m_nLastProgressPercent;

	bool ishltv;

	char pad5[0x12B];                               // 0x4CB8
	void* m_events;                    // 0x4DEC
};
