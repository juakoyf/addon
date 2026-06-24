#include "PatternScan.hpp"
#include <Windows.h>
#include <vector>
#include <cctype>
#include <cstdlib>
#include <cstring>

namespace Memory {

    static std::vector<int> PatternToBytes(const std::string& pattern) {
        std::vector<int> bytes;
        bytes.reserve(pattern.length());

        const char* start = pattern.c_str();
        const char* end = start + pattern.length();

        for (const char* cur = start; cur < end; ++cur) {
            if (*cur == ' ')
                continue;

            if (*cur == '?') {
                ++cur;
                if (cur < end && *cur == '?')
                    ++cur;
                bytes.push_back(-1);
            }
            else if (std::isxdigit(static_cast<unsigned char>(*cur))) {
                char byteStr[3]{ *cur, *(cur + 1), 0 };
                bytes.push_back(std::strtoul(byteStr, nullptr, 16));
                ++cur;
            }
        }

        return bytes;
    }

    std::uintptr_t Scan(std::uintptr_t moduleBase, const std::string& signature) {
        if (!moduleBase)
            return 0;

        const auto dos = reinterpret_cast<PIMAGE_DOS_HEADER>(moduleBase);
        if (!dos || dos->e_magic != IMAGE_DOS_SIGNATURE)
            return 0;

        const auto nt = reinterpret_cast<PIMAGE_NT_HEADERS>(
            moduleBase + dos->e_lfanew
            );
        if (!nt || nt->Signature != IMAGE_NT_SIGNATURE)
            return 0;

        const size_t sizeOfImage = nt->OptionalHeader.SizeOfImage;
        if (!sizeOfImage)
            return 0;

        const auto patternBytes = PatternToBytes(signature);
        if (patternBytes.empty())
            return 0;

        const auto* scanBytes = reinterpret_cast<const std::uint8_t*>(moduleBase);
        const size_t patternSize = patternBytes.size();

        for (size_t i = 0; i <= sizeOfImage - patternSize; ++i) {
            bool found = true;

            for (size_t j = 0; j < patternSize; ++j) {
                if (patternBytes[j] != -1 &&
                    scanBytes[i + j] != static_cast<std::uint8_t>(patternBytes[j])) {
                    found = false;
                    break;
                }
            }

            if (found)
                return moduleBase + i;
        }

        return 0;
    }

    std::uintptr_t Scan(const std::string& moduleName, const std::string& signature) {
        const HMODULE module = GetModuleHandleA(moduleName.c_str());
        if (!module)
            return 0;

        return Scan(reinterpret_cast<std::uintptr_t>(module), signature);
    }
}
