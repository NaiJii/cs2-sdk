#pragma once

#include <cstdint>
#include <cstdio>

#include "../utils/console/console.hpp"
#include "../defines.hpp"

#define CALL_VIRTUAL(retType, idx, ...) \
    vmt::CallVirtual<retType>(idx, __VA_ARGS__)

namespace vmt {
    template <typename T = void*>
    inline T GetVMethod(uint32_t uIndex, void* pClass) {
        if (!pClass) {
            LOG("Tried getting virtual function from a null class.\n");
            return T{};
        }

        void** pVTable = *static_cast<void***>(pClass);
        if (!pVTable) {
            LOG("Tried getting virtual function from a null vtable.\n");
            return T{};
        }

        return reinterpret_cast<T>(pVTable[uIndex]);
    }

    template <typename T, typename... Args>
    inline T CallVirtual(uint32_t uIndex, void* pClass, Args... args) {
        using FnType = T(__thiscall*)(void*, Args...);
        auto pFunction = static_cast<FnType>(GetVMethod(uIndex, pClass));
        if (!pFunction) {
            LOG("Tried calling a null virtual function.\n");
            return T{};
        }

        return pFunction(pClass, args...);
    }
}  // namespace vmt
