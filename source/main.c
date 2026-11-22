#include <stdio.h>
#include <switch.h>

int main(int argc, char **argv) {
    consoleInit(NULL);
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);

    printf("Pokopia-NX | Switch 1 feasibility prototype\n");
    printf("-----------------------------------------\n");
    printf("This is NOT a game port.\n");
    printf("Hardware target: Switch / Switch Lite\n");
    printf("Goal: verify native libnx NRO execution.\n\n");
    printf("Press PLUS (+) to exit.\n");

    while (appletMainLoop()) {
        padUpdate(&pad);
        if (padGetButtonsDown(&pad) & HidNpadButton_Plus) break;
        consoleUpdate(NULL);
    }
    consoleExit(NULL);
    return 0;
}
