// G4: run authentic nxrecomp-generated code from the G3 self-authored fixture.
// Copyright (c) Pokopia-NX contributors. GPL-3.0-or-later.
#include <switch.h>
#include <cstdio>
#include <cstdint>
#include <nxrecomp/ctx.h>
#include "funcs.h"

int main() {
    consoleInit(nullptr);
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);

    Ctx c = {};
    c.x[0] = 12;
    c.x[1] = 30;
    f_400000(&c); // tiny_sum: ADD(S) x0, x0, x1; RET
    const bool sum_ok = c.x[0] == 42;
    std::printf("G4 genuine nxrecomp output\n");
    std::printf("SUM 12 + 30 = %llu: %s\n", (unsigned long long)c.x[0], sum_ok ? "PASS" : "FAIL");

    c.x[0] = 50;
    c.x[1] = 8;
    f_400008(&c); // tiny_difference: SUB(S) x0, x0, x1; RET
    const bool diff_ok = c.x[0] == 42;
    std::printf("DIFF 50 - 8 = %llu: %s\n", (unsigned long long)c.x[0], diff_ok ? "PASS" : "FAIL");
    std::printf("%s\n", sum_ok && diff_ok ? "ALL PASS" : "FAILED");
    std::printf("Synthetic input; NO Pokopia game code.\n");
    std::printf("PLUS to exit.\n");

    while (appletMainLoop()) {
        padUpdate(&pad);
        if (padGetButtonsDown(&pad) & HidNpadButton_Plus) break;
        consoleUpdate(nullptr);
    }
    consoleExit(nullptr);
    return (sum_ok && diff_ok) ? 0 : 1;
}
