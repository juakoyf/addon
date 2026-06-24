#pragma once
#include <string>
#include <cstdint>

struct datamap_t;

namespace Memory {
    std::uintptr_t Scan(std::uintptr_t moduleBase, const std::string& signature);
    std::uintptr_t Scan(const std::string& moduleName, const std::string& signature);
}
