// Pokopia-NX G1: standalone ABI header smoke test.
// Only uses public data types/macros; does not run game code.
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <switch.h>
#include <nxrecomp/ctx.h>

int main(void) {
    consoleInit(NULL);
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);

    Ctx ctx = {0};
    ctx.x[0] = 0x12345678ULL;
    ctx.x[1] = 0x11111111ULL;
    ctx.v[0].u32[0] = 0xCAFEU;
    ctx.N = 1;
    const uint32_t id = NX_IMPORT_ID(3, 42);

    const int ok =
        (ctx.x[0] + ctx.x[1] == 0x23456789ULL) &&
        (ctx.v[0].u32[0] == 0xCAFEU) &&
        (ctx.N == 1) &&
        (NX_IMPORT_MODULE(id) == 3) &&
        (NX_IMPORT_SYM(id) == 42) &&
        (sizeof(V128) == 16) &&
        (NXRECOMP_ABI_VERSION >= 3);

    printf("Pokopia-NX: nxrecomp ABI probe\n");
    printf("Header ABI version: %d\n", NXRECOMP_ABI_VERSION);
    printf("Ctx bytes: %lu | V128 bytes: %lu\n",
           (unsigned long)sizeof(Ctx), (unsigned long)sizeof(V128));
    printf("Register/vector/import tests: %s\n", ok ? "PASS" : "FAIL");
    printf("Not a Pokopia port. Press PLUS to exit.\n");

    while (appletMainLoop()) {
        padUpdate(&pad);
        if (padGetButtonsDown(&pad) & HidNpadButton_Plus) break;
        consoleUpdate(NULL);
    }
    consoleExit(NULL);
    return ok ? 0 : 1;
}
