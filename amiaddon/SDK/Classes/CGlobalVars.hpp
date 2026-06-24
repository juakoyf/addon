#pragma once

class CGlobalVarsBase {
public:
	float realtime = 0.0f;
	int framecount = 0;
	float absoluteframetime = 0.0f;
	float absoluteframestarttimestddev = 0.0f;
	float curtime = 0.0f;
	float frametime = 0.0f;
	int maxClients = 0;
	int tickcount = 0;
	float interval_per_tick = 0.0f;
	float interpolation_amount = 0.0f;
	int simTicksThisFrame = 0;
	int network_protocol = 0;
	void* pSaveData = nullptr;
	bool m_bClient = false;
	bool m_remote_client = false;
	int nTimestampNetworkingBase = 0;
	int nTimestampRandomizeWindow = 0;
};

class CGlobalVars : public CGlobalVarsBase {
public:

};