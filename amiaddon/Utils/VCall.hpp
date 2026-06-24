#pragma once
#include <cstddef>
#include <cstdint>

namespace Memory
{
    template <typename Fn>
    __forceinline Fn VCall(const void* instance, std::size_t index) noexcept
    {
        if (!instance)
            return nullptr;

        auto vtable = *reinterpret_cast<void***>(const_cast<void*>(instance));
        if (!vtable)
            return nullptr;

        return reinterpret_cast<Fn>(vtable[index]);
    }
}
