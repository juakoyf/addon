#pragma once
#include <cstdint>

class CFontManager {
public:
	__forceinline char*& GetLanguage() {
		return *(char**)((uintptr_t)this + 0x28);
	}
};