// SPDX-License-Identifier: GPL-3.0-or-later
// G6: legitimate nxrecomp-lifted guest loads/stores via a checked host-pointer bridge.
// NOT native guest virtual memory mapping; no Pokemon game code.
#include <switch.h>
#include <nxrecomp/ctx.h>
#include "funcs.h"
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace {
constexpr uint64_t kGuestBase = 0x7100000000ULL;
constexpr size_t kRegionSize = 64 * 1024;
alignas(16) std::array<uint8_t, kRegionSize> memory{};

// This checks and translates ONE guest-provided address before entering lifted code.
// The lifted function receives a genuine host pointer. Guest-internal pointer
// arithmetic/loads at untranslated addresses are NOT supported by this method.
bool guestToHost(uint64_t guest, size_t size, size_t alignment, uint64_t& host) {
    host = 0;
    if (!alignment || (alignment & (alignment - 1))) return false;
    if (guest < kGuestBase || (guest & (alignment - 1))) return false;
    const uint64_t offset = guest - kGuestBase;
    if (offset > kRegionSize || size > kRegionSize - static_cast<size_t>(offset)) return false;
    host = reinterpret_cast<uint64_t>(memory.data() + static_cast<size_t>(offset));
    return true;
}

void show(const char* label, bool success, int& count) {
    if (success) ++count;
    std::printf("%-25s %s\n", label, success ? "PASS" : "FAIL");
}
} // namespace

int main() {
    consoleInit(nullptr);
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);

    int passed = 0;
    uint64_t host = 0;
    Ctx c = {};

    const bool mapped64 = guestToHost(kGuestBase, 8, 8, host);
    show("64-bit checked bridge", mapped64 && host != 0, passed);
    if (mapped64) {
        uint64_t initial = 12;
        std::memcpy(reinterpret_cast<void*>(host), &initial, sizeof(initial));
        c.x[0] = host;
        c.x[1] = 30;
        f_400000(&c); // GENERATED from g6_add64: LDR; ADD; STR; RET
        uint64_t result = 0;
        std::memcpy(&result, reinterpret_cast<void*>(host), sizeof(result));
        show("Lifted LDR/STR 64", result == 42, passed);
    } else show("Lifted LDR/STR 64", false, passed);

    const bool mapped32 = guestToHost(kGuestBase + kRegionSize - 4, 4, 4, host);
    show("32-bit last valid slot", mapped32 && host != 0, passed);
    if (mapped32) {
        uint32_t initial = 58;
        std::memcpy(reinterpret_cast<void*>(host), &initial, sizeof(initial));
        c = {};
        c.x[0] = host;
        c.x[1] = 42;
        f_400010(&c); // GENERATED from g6_add32
        uint32_t result = 0;
        std::memcpy(&result, reinterpret_cast<void*>(host), sizeof(result));
        show("Lifted LDR/STR 32", result == 100, passed);
    } else show("Lifted LDR/STR 32", false, passed);

    show("Reject below base", !guestToHost(kGuestBase - 8, 8, 8, host), passed);
    show("Reject edge crossing", !guestToHost(kGuestBase + kRegionSize - 4, 8, 4, host), passed);
    show("Reject unaligned", !guestToHost(kGuestBase + 1, 8, 8, host), passed);
    show("Reject huge address", !guestToHost(UINT64_MAX, 8, 8, host), passed);

    std::printf("\nPokopia-NX G6 memory bridge\n");
    std::printf("%d/8 checks passed\n", passed);
    std::printf("%s\n", passed == 8 ? "ALL PASS" : "TEST FAILURE");
    std::printf("No real guest VM / Pokopia code.\n");
    std::printf("PLUS to exit.\n");
    while (appletMainLoop()) {
        padUpdate(&pad);
        if (padGetButtonsDown(&pad) & HidNpadButton_Plus) break;
        consoleUpdate(nullptr);
    }
    consoleExit(nullptr);
    return passed == 8 ? 0 : 1;
}
