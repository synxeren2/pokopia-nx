# G3 — Genuine nxrecomp lifter experiment

**Status:** GitHub Actions pipeline authored; successful execution not yet verified.

## Objective

Prove that the real [Recompia nxrecomp](https://github.com/Dittobyte/Recompia/tree/main/recomp) AArch64 lifter can translate **our self-authored assembly code** into C++ on a host CI machine.

This is the first test in Pokopia-NX that invokes the real `python -m nxrecomp lift` command. It deliberately does **not** claim that the resulting code executes on Switch Lite until a separate devkitA64 compile and runtime test is built.

## Inputs and method

- [Assembly fixture](../probes/g3/tiny_aarch64.S): tiny_sum (`ADDS`), tiny_difference (`SUBS`), `_start` entrypoint with a branch.
- [CI workflow](../.github/workflows/g3-lift.yml) uses Linux AArch64 cross assembler/linker to make a static ELF64 AArch64 binary with symbols.
- Recompia is fetched from GitHub; its exact current commit SHA is saved as build evidence.
- Python dependencies are installed in a temporary venv from `recomp/requirements.txt`.
- The actual invocation is `python -m nxrecomp lift tiny.elf lifted/ --kind static_elf --game generic --name g3tiny --jobs 1 --shards 1 --locals off`.
- Output checks require `funcs.h`, `module_g3tiny.cpp` and at least one nonempty `c_*.cpp` translation unit.

The artifact `pokopia-nx-g3-lifted-cpp` should contain disassembly, generated C++ and the Recompia commit. **No NRO is produced by this G3 pipeline yet.**

## Why G3 matters

G1 checked `Ctx` declarations; G2 called existing CPU helpers from hand-written C. G3 tests the lifter itself, including ELF parsing, function discovery, instruction decoding and C++ emission. We still need a **G4 generated-code integration** that links the actual generated `c_*.cpp` with a small runtime implementing necessary entry points and executes its function table on Switch 1.

## Go/no-go

- **Go:** GitHub Actions succeeds and the artifact includes real generated C++ referencing expected ARM64 functions.
- **Investigate:** failure from ELF symbol discovery, unsupported opcodes, Python dependency installation, output naming or an actual lifter bug. Read CI logs; do not claim the pipeline passed.
- **Stop:** the generated output depends on unsupported compiler/ABI primitives that cannot be adapted safely and efficiently for the target.

## Important limitations

An ordinary static ELF built on Ubuntu is a legal, self-authored fixture, **not a Nintendo Switch 2 game binary**. No proprietary game files, update contents, keys, extracted textures, or shader binaries are involved. Passing G3 does not prove CPU service virtualization, Horizon OS loading, NVN2→deko3d graphics or a viable Pokopia port.

Next: verify output, determine which runtime-provided declarations are actually referenced, then create a small self-contained libnx C++ harness for those generated functions.
