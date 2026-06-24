#pragma once
#include "../SDK/Interfaces/Interfaces.hpp"


class ClientClass;

class IBaseClientDLL {
public:
	int m_IServerTick;


	ClientClass* GetAllClasses();
	bool IsChatRaised();
};