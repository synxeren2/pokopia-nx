// SPDX-License-Identifier: GPL-3.0-or-later
// G5: bounded guest-address translation, NOT actual Horizon VM reservation.
#include <switch.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <array>
#include <limits>

namespace {
constexpr uint64_t kGuestBase = 0x7100000000ULL;
constexpr size_t kSize = 64 * 1024;
alignas(16) std::array<uint8_t, kSize> backing{};

bool translate(uint64_t guest, size_t bytes, size_t align, uint8_t** host) {
    if (!host) return false;
    *host = nullptr;
    if (!align || (align & (align - 1)) != 0) return false;
    if (guest < kGuestBase || (guest & (align - 1)) != 0) return false;
    const uint64_t offset = guest - kGuestBase; // safe after lower-bound check
    if (offset > kSize || bytes > kSize - static_cast<size_t>(offset)) return false;
    *host = backing.data() + static_cast<size_t>(offset);
    return true;
}

bool guest_write32(uint64_t addr, uint32_t value) {
    uint8_t* dst = nullptr;
    if (!translate(addr, sizeof(value), alignof(uint32_t), &dst)) return false;
    std::memcpy(dst, &value, sizeof(value));
    return true;
}
bool guest_read32(uint64_t addr, uint32_t* value) {
    if (!value) return false;
    uint8_t* src = nullptr;
    if (!translate(addr, sizeof(*value), alignof(uint32_t), &src)) return false;
    std::memcpy(value, src, sizeof(*value));
    return true;
}
bool guest_write64(uint64_t addr, uint64_t value) {
    uint8_t* dst = nullptr;
    if (!translate(addr, sizeof(value), alignof(uint64_t), &dst)) return false;
    std::memcpy(dst, &value, sizeof(value));
    return true;
}
bool guest_read64(uint64_t addr, uint64_t* value) {
    if (!value) return false;
    uint8_t* src = nullptr;
    if (!translate(addr, sizeof(*value), alignof(uint64_t), &src)) return false;
    std::memcpy(value, src, sizeof(*value));
    return true;
}
void result(const char* label, bool ok, int& passed, int& total) {
    ++total;
    passed += ok ? 1 : 0;
    std::printf("%-25s %s\n", label, ok ? "PASS" : "FAIL");
}
} // namespace

int main() {
    consoleInit(nullptr);
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);

    int passed = 0, total = 0;
    uint32_t read32 = 0;
    uint64_t read64 = 0;
    uint8_t* ptr = nullptr;
    bool ok = guest_write32(kGuestBase, 0x12345678U) &&
              guest_read32(kGuestBase, &read32) && read32 == 0x12345678U;
    result("32-bit store and load", ok, passed, total);

    ok = guest_write64(kGuestBase + kSize - 8, 0x1122334455667788ULL) &&
         guest_read64(kGuestBase + kSize - 8, &read64) &&
         read64 == 0x1122334455667788ULL;
    result("64-bit last valid slot", ok, passed, total);

    result("Reject below base", !guest_read32(kGuestBase - 4, &read32), passed, total);
    result("Reject beyond region", !guest_write32(kGuestBase + kSize, 1), passed, total);
    result("Reject crossing edge", !translate(kGuestBase + kSize - 4, 8, 4, &ptr), passed, total);
    result("Reject misalignment", !guest_write32(kGuestBase + 1, 1), passed, total);
    result("Reject overflow addr", !translate(UINT64_MAX, 8, 1, &ptr), passed, total);
    result("Reject bad alignment", !translate(kGuestBase, 4, 3, &ptr), passed, total);
    result("Reject null output", !translate(kGuestBase, 4, 4, nullptr), passed, total);
    result("Accept final byte", translate(kGuestBase + kSize - 1, 1, 1, &ptr) && ptr == backing.data() + kSize - 1, passed, total);

    std::printf("\nPokopia-NX G5 bounded memory\n");
    std::printf("Guest base: 0x%llx, capacity: %zu KiB\n", (unsigned long long)kGuestBase, kSize / 1024);
    std::printf("%d/%d tests passed\n", passed, total);
    std::printf("%s\n", passed == total ? "ALL PASS" : "TEST FAILURE");
    std::printf("Software translation only; no real VM mapping.\n");
    std::printf("PLUS to exit.\n");
    while (appletMainLoop()) {
        padUpdate(&pad);
        if (padGetButtonsDown(&pad) & HidNpadButton_Plus) break;
        consoleUpdate(nullptr);
    }
    consoleExit(nullptr);
    return passed == total ? 0 : 1;
}
