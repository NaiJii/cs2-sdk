#include "pch.hpp"

#include <sdl/sdl.hpp>

#include <memory/memory.hpp>
#include <constants/constants.hpp>

namespace sdl {
    int SetRelativeMouseMode(int enabled) {
        static const auto fn = CMemory::GetProcAddress(CConstants::SDL_LIB, "SDL_SetRelativeMouseMode");
        return fn.Call<decltype(&SetRelativeMouseMode)>(enabled);
    }

    int SetWindowMouseGrab(void* window, int grabbed) {
        static const auto fn = CMemory::GetProcAddress(CConstants::SDL_LIB, "SDL_SetWindowMouseGrab");
        return fn.Call<decltype(&SetWindowMouseGrab)>(window, grabbed);
    }

    void WarpMouseInWindow(void* window, float x, float y) {
        static const auto fn = CMemory::GetProcAddress(CConstants::SDL_LIB, "SDL_WarpMouseInWindow");
        return fn.Call<decltype(&WarpMouseInWindow)>(window, x, y);
    }
}  // namespace sdl
