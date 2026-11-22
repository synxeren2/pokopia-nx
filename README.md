# Pokopia-NX

> Experimental research project exploring the feasibility of running **Pokémon Pokopia** on the original Nintendo Switch (including Switch Lite).

**Status: Planning / feasibility study.** This repository does **not** contain a playable port, game binaries, or a working Nintendo Switch build.

## Goal

Investigate whether the game could be adapted to Nintendo Switch 1 hardware using reverse engineering, static recompilation, and homebrew development techniques.

Provisional *research targets* (not supported features):
- 30 FPS target
- 480p or lower internal 3D rendering resolution
- Reduced draw distance, LOD, shadows, and effects
- Memory optimization for the original Switch's 4 GB RAM
- A native Horizon OS / libnx application, if technically feasible

Lowering resolution alone will not solve compatibility, memory, CPU, or GPU architecture differences.

## Roadmap

- [ ] Evaluate technical feasibility and document critical blockers
- [ ] Study relevant open-source recompilation and Switch homebrew projects
- [ ] Set up devkitPro, devkitA64, libnx, and a reproducible build environment
- [ ] Build and test a minimal Nintendo Switch `.nro` homebrew prototype
- [ ] Investigate runtime, system API, and graphics translation requirements
- [ ] Profile memory and performance on real Switch 1 hardware
- [ ] Decide whether a game port is realistically achievable

## Related work

These are **references for study**, not included dependencies or confirmed compatibility layers:

- [Dittobyte/Recompia](https://github.com/Dittobyte/Recompia) — game recompilation research
- [SwitchWakerHD](https://github.com/centollOS/SwitchWakerHD) — Switch porting architecture reference
- [devkitPro](https://devkitpro.org/) — homebrew toolchain
- [libnx](https://github.com/switchbrew/libnx) — Nintendo Switch homebrew library

## Development principles

1. Start with a feasibility report and small testable prototypes.
2. Keep source code, build scripts, and reverse-engineering notes organized.
3. Clearly distinguish verified hardware results from hypotheses.
4. Use AI tools for code analysis and prototyping, but manually verify generated changes.
5. Do not commit copyrighted game files, encryption keys, firmware, or proprietary assets.

## Legal note

This is an **unofficial, independent research project** and is not affiliated with or endorsed by Nintendo, The Pokémon Company, or the game's developers. Contributors must use legally obtained materials and respect applicable law and third-party licenses. No game content is provided here.

## Current state

No port is available yet. The first milestone is compiling and running a minimal Switch 1 homebrew application and documenting whether further work is feasible.

## GPU research prototype

A separate GitHub Actions workflow, [Build deko3d GPU probe](.github/workflows/gpu-probe.yml), builds an experimental 854×480 deko3d triangle test based on the official switchbrew sample. This is **not** a game port and has **not** been verified on Switch Lite hardware.

See [graphics research and test plan](docs/graphics-research.md) for evidence, caveats, and the next experiments.

## Milestone 3: rotating 3D cube

Use the [cube test workflow](.github/workflows/cube-test.yml) to compile the official deko3d example suite and select **03: Cube** in the resulting NRO. See [test instructions and verification criteria](docs/rotating-cube.md). This is an experimental graphics test, not a playable port.

## Recompia compatibility investigation

Read the [source-grounded Switch 1 feasibility report](docs/recompia-switch1-feasibility.md). The first isolated test is the [nxrecomp ABI probe](.github/workflows/abi-probe.yml): it builds a small homebrew NRO from the upstream `ctx.h` interface. This does **not** load game code or prove the complete runtime can run on Switch.

## G2: synthetic nxrecomp CPU helper execution

The [G2 CPU probe](.github/workflows/cpu-helper-probe.yml) builds a standalone NRO exercising real nxrecomp arithmetic, condition and memory helper functions through a **manually authored** synthetic function. [Read the experiment details](docs/cpu-helper-g2.md). This is not lifted Pokopia code; runtime execution on Switch Lite is not yet verified.

## G3: actual nxrecomp lift

The [G3 GitHub Actions pipeline](.github/workflows/g3-lift.yml) converts a self-authored [AArch64 assembly fixture](probes/g3/tiny_aarch64.S) into C++ using Recompia's **real** `nxrecomp lift` command. [See scope and pass criteria](docs/g3-real-lifter.md). This workflow currently produces a generated-C++ artifact, **not a Switch NRO**, and must be verified before claiming a successful lift.

## G4: execute real lifted output on Switch Lite

The [G4 workflow](.github/workflows/g4-lifted-nro.yml) regenerates C++ with the **actual nxrecomp lifter**, compiles that generated source into a homebrew NRO, and tests two self-authored arithmetic routines. See [G4 test plan](docs/g4-generated-on-switch.md). This is **not a game port**; successful compilation and hardware execution remain unverified.

## G5: safe guest-address translation

[Build the G5 memory probe](.github/workflows/g5-memory.yml) to test a bounded 64 KiB software memory mapping (10 checks) on Switch 1. Read the [G5 report](docs/g5-bounded-memory.md). This is **not** an actual Horizon OS virtual-memory mapping and does not yet integrate with generated game code.

## G6: lifted memory access through a checked bridge

[Build the G6 Switch NRO](.github/workflows/g6-memory-bridge.yml) to execute genuine nxrecomp-generated 64/32-bit LDR/STR operations against a small memory buffer with checked guest-to-host translation. See [G6 research and caveats](docs/g6-memory-bridge.md). This is a smoke test, **not** a general guest virtual-memory implementation or a playable game port.
