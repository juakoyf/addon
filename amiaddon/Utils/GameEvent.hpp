#pragma once
#include "../SDK/Classes/IGameEventManager.hpp"
#include <vector>
#include <string>
#include <utility>


class GameEvent : public IGameEventListener
{
public:
	void Register();
	void Shutdown();

public: // IGameEventListener
	virtual void FireGameEvent(IGameEvent* event);
	virtual int  GetEventDebugID(void);
};

extern std::vector<std::string> m_vecRagebait;

extern GameEvent g_GameEvent;
