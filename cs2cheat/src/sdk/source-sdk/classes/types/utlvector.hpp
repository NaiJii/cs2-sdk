#pragma once

#include "../interfaces/cheapmemalloc.hpp"

#include "utlmap.hpp"

template <typename T>
class CUtlVector {
   public:
    inline bool Empty() const { return m_size == 0; }

    int m_size;
    char pad0[0x4];  // no idea
    T* m_data;
    char pad1[0x8];  // no idea
};
