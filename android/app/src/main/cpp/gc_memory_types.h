#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

// Host-safe types for values that represent GameCube/emulated addresses.
// These are intentionally NOT raw host pointers. A 32-bit emulated address
// must remain 32-bit even when the Android host is AArch64.
using GcAddr = std::uint32_t;
using GcOffset = std::uint32_t;

static_assert(sizeof(GcAddr) == 4, "GameCube addresses must stay 32-bit");
static_assert(std::is_unsigned<GcAddr>::value, "GameCube address type must be unsigned");

// Native host pointer/integer used only when dealing with Android/CPU memory.
using HostInt = std::uintptr_t;

namespace gc {

inline bool is_null(GcAddr address) {
    return address == 0;
}

} // namespace gc
