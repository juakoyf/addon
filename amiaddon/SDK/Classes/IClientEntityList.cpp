#include "IClientEntityList.hpp"
#include "../Utils/VCall.hpp"
#include "CBaseHandle.hpp"

int IClientEntityList::GetHighestEntityIndex()
{
	using Fn = int(__thiscall*)(void*);
	return Memory::VCall<Fn>(this, 6)(this);
}

IClientEntity* IClientEntityList::GetClientEntity(int entnum)
{
	using Fn = IClientEntity * (__thiscall*)(void*, int);
	return Memory::VCall<Fn>(this, 3)(this, entnum);
}

IClientEntity* IClientEntityList::GetClientEntityFromHandle(CBaseHandle hEnt)
{
	using Fn = IClientEntity * (__thiscall*)(void*, CBaseHandle);
	return Memory::VCall<Fn>(this, 4)(this, hEnt);
}
