#include "IBaseClientDll.hpp"


ClientClass* IBaseClientDLL::GetAllClasses()
{
	using Fn = ClientClass * (__thiscall*)(void*);
	return Memory::VCall<Fn>(this, 8)(this);
}

bool IBaseClientDLL::IsChatRaised() {
	using Fn = bool(__thiscall*)(void*);
	return Memory::VCall<Fn>(this, 89)(this);
}
