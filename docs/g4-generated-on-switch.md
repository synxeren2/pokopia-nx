# G4 — Run genuine nxrecomp-generated code on Nintendo Switch 1

**State:** Pipeline committed, not yet verified by a successful GitHub Actions run or physical hardware test.

## Previous gate

The user reported the G3 `nxrecomp lift` workflow succeeded for the self-authored AArch64 ELF. That validates the code generation stage only; this G4 stage attempts compilation and execution on Switch Lite.

## Two-job reproducible pipeline

1. **Generate** (Ubuntu): assemble/link `probes/g3/tiny_aarch64.S` as a static AArch64 ELF and run the real Recompia `python -m nxrecomp lift`. Save the exact Recompia commit SHA used.
2. **Build NRO** (devkitPro): compile the produced `c_*.cpp` source and `funcs.h` alongside `probes/g4/main.cpp` using genuine `nxrecomp/cxx.h`, `ctx.h`, `fpexact.h` and `neon.h`. Exclude the module metadata translation unit for this narrowly scoped experiment; unneeded functions are discarded through `--gc-sections`.

The harness calls:
- `f_400000(&c)` from actual generated code: computes 12 + 30 and expects 42.
- `f_400008(&c)` from actual generated code: computes 50 - 8 and expects 42.

The pipeline asserts these named locations exist in the source ELF and the generated declarations.

## Instructions

Run [G4 - run genuine lifted code on Switch 1](../.github/workflows/g4-lifted-nro.yml) in GitHub Actions. If successful, download `pokopia-nx-g4-genuine-lift`, extract `pokopia-nx-g4.nro` and launch through Homebrew Menu on the Switch Lite. Both cases must show `PASS` followed by `ALL PASS`.

### What a pass proves

The actual nxrecomp lifter's translated **self-authored**, small AArch64 arithmetic functions can compile for devkitA64 and execute with correct output through `Ctx` on a physical original Switch. This would be the first end-to-end source → machine code → recompiled C++ → native Switch probe.

### What it does NOT prove

No proprietary Pokopia NSO is translated. No system calls, game imports, runtime loader, GPU engine, NVN2 shaders or complete virtual memory mappings are exercised. This does not validate game startup, battery life, stability, 30 FPS or even that large generated modules fit in RAM.

### Potential failure modes

- New upstream Recompia revision changes the generated ABI or entry-function names.
- Generated functions may need unresolved runtime callbacks even in the tiny fixture.
- C++20 features in `cxx.h` may not be supported by the available devkitPro compiler.
- The generic ELF function-discovery path may select unexpected addresses.

Do not modify successful G0–G2 test builds to work around G4 failures. Record the exact failing step and log and revise this separate workflow.

This workflow currently fetches the upstream moving main branch; **pin a tested commit before claiming the build is reproducible**. All fixture input is self-authored, and no Nintendo game content is committed.

## Future work

A passing G4 is a prerequisite for a safely bounded guest-memory translation experiment and an OS interface test. Focus on realistic limitations before attempting any game-specific integration.

## Verified physical-device result — 2026-11-22

The user provided a photograph of the G4 NRO running on a physical Nintendo Switch Lite. The display reads:

```text
G4 genuine nxrecomp output
SUM 12 + 30 = 42: PASS
DIFF 50 - 8 = 42: PASS
ALL PASS
Synthetic input: NO Pokopia game code.
PLUS to exit.
```

**Outcome: G4 passed on user hardware.** The self-authored AArch64 input was lifted by actual nxrecomp to C++, built into a Switch-native NRO, and the generated arithmetic functions produced expected results on hardware. The source is a small legal synthetic fixture, not a game executable. No Recompia guest services, graphics translation, complete memory layout, or Pokopia startup have been tested.

**Recommended next step:** G5, an isolated bounded guest-address-space mapping and small memory semantics probe; preserve the working G4 workflow. Before using addresses from real game modules, measure contiguous virtual address availability, backing memory, and Horizon/libnx constraints. Never allocate multi-gigabyte buffers for a quick smoke test.
