#pragma once

class IClientEntity;
class CBaseHandle;

class IClientEntityList
{
public:
	IClientEntity* GetClientEntity(int entnum);
	IClientEntity* GetClientEntityFromHandle(CBaseHandle hEnt);
	int GetHighestEntityIndex();
};
