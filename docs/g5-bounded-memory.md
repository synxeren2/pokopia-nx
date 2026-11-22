# G5 — bounded guest address translation

**Status:** Code and CI pipeline committed; compilation and Switch Lite runtime not yet verified.

## Scope

The verified G4 fixture executed actual `nxrecomp`-lifted arithmetic functions on a physical Switch Lite. G5 isolates an equally important requirement: safely interpreting **guest addresses** without ever dereferencing arbitrary guest pointers.

In `probes/g5/main.cpp`, a fixed 64 KiB, 16-byte aligned **host-owned array** represents a fake guest memory region starting at `0x7100000000`. A simple translation function maps `guest address → checked offset → host array`. No Horizon OS virtual-memory reservation, process address remapping, permissions enforcement, guest loader or Nintendo game assets are involved.

### Test cases

1. 32-bit write/read
2. 64-bit write/read in the last valid aligned slot
3. Reject address below mapped base
4. Reject address after region end
5. Reject access spanning past region end
6. Reject misaligned 32-bit write
7. Reject extreme address `UINT64_MAX`
8. Reject invalid alignment
9. Reject null output pointer
10. Accept valid final byte

The implementation checks bounds by subtraction, avoiding `address + size` overflow. A successful run prints **10/10 tests passed** and **ALL PASS**.

## Build and device test

- [GitHub Actions workflow](../.github/workflows/g5-memory.yml) — `G5 - bounded guest memory probe`
- Download `pokopia-nx-g5-memory` artifact and extract `pokopia-nx-g5.nro`.
- Run in Homebrew Menu on your Switch Lite. Photograph the PASS summary and report any failed case.

## Limitations and next step

**G5 is a software address-translation experiment, not a working Recompia memory subsystem.** The actual `nxrecomp/ctx.h` memory helpers currently treat guest addresses as host addresses (`rd32/wr32` directly dereference pointers). Those helpers cannot use this isolated translation interface without explicit integration or mapped guest memory. The next engineering decision is whether to allocate/protect an address window on Horizon OS and ensure addresses are compatible, or to introduce a translation-aware abstraction for generated memory accesses.

Before selecting a solution, investigate libnx/Horizon virtual-memory APIs, process address-space restrictions, and the cost of translating every access. Do **not** attempt arbitrary fixed-address mappings or large allocations on a user's device without a smaller verified design.

This experiment does not measure gameplay performance, Switch 2/NVN2 compatibility, or Pokopia's RAM usage.
