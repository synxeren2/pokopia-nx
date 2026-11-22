# G6 — real lifted load/store with a bounded memory bridge

**State:** source and CI committed; successful compilation and physical hardware execution are not yet verified.

## Why this is different from G5

G5 tested only manually written C++ memory functions. In G6, **the AArch64 instructions `LDR`, `ADD`, and `STR` are authored in a tiny standalone assembly fixture and lifted into C++ by the actual Recompia `nxrecomp` tool**. A separate harness provides valid host addresses to the generated functions.

Input: [probes/g6/memory_ops.S](../probes/g6/memory_ops.S)  
Harness: [probes/g6/main.cpp](../probes/g6/main.cpp)  
CI: [g6-memory-bridge.yml](../.github/workflows/g6-memory-bridge.yml)

### Translation mechanism

A fixed 64 KiB host-owned backing buffer represents guest addresses starting at `0x7100000000`. A checking function verifies a guest address, size and alignment, then derives its **host pointer**. Only after successful checks does the harness put this host pointer into `Ctx.x[0]` and call the **actual lifter-generated** function.

This differs fundamentally from Recompia's normal flat guest mapping: the generated code sees a *host pointer*, not the original guest virtual address. A real game might compute additional guest addresses inside lifted functions; this one-time boundary translation **cannot** intercept those internal accesses. Such accesses would still be unsafe without a proper generated-code memory abstraction or valid guest-host mappings.

### Eight checks

1. Accept 64-bit checked guest → host translation.
2. Execute generated 64-bit `LDR/ADD/STR`; 12 + 30 → 42.
3. Accept the 32-bit final slot of the 64 KiB region.
4. Execute generated 32-bit `LDR/ADD/STR`; 58 + 42 → 100.
5. Reject below-base guest address.
6. Reject 64-bit access crossing the region's end.
7. Reject misaligned access.
8. Reject extreme invalid address.

The assembly function addresses are fixed by the fixture and verified during CI (`0x400000` and `0x400010`). Both generated function calls are linked into a standalone devkitA64/libnx NRO.

**Expected device summary:** `8/8 checks passed` and `ALL PASS`.

## Testing instructions

1. Open [G6 Actions workflow](../.github/workflows/g6-memory-bridge.yml), run it on `main`.
2. Verify BOTH `lift` and `build-nro` jobs show green checks.
3. Download `pokopia-nx-g6-memory-bridge`, extract `pokopia-nx-g6.nro`.
4. Run the NRO in Switch Lite Homebrew Menu, record a photo and any failing check.

## Boundaries and follow-ups

- This tests a synthetic, self-authored ARM64 ELF, not any Pokopia binary or game asset.
- It does **not** reserve any real Horizon guest virtual address window, enforce virtual memory permissions, measure memory occupancy, translate arbitrary pointer arithmetic, emulate Horizon services, or implement NVN2→deko3d.
- Do not treat success as a fully functional Recompia guest-memory manager.
- Next examine `nxrecomp/cxx.h` and `ctx.h` raw memory helpers, and determine whether all generated reads/writes can be routed through a bounded address-translation interface. Test a guest-computed offset after that.
- The build currently pulls Recompia from its current branch and logs the commit SHA; pin the source revision after a successful run for reproducibility.
