# Recompia → Switch 1 compatibility assessment

**Date:** 2026-11-22  
**Status:** architecture review only — not a proof of Pokopia compatibility  
**Target:** Nintendo Switch Lite (Horizon OS, Tegra X1, 4 GB physical RAM)

## Executive result

**A direct Recompia build for Switch 1 is currently not supported.** The obstacle is not simply frame rate or pixel count: platform-specific system calls, GPU API translation, virtual memory layout, build/link targets and total memory use all require work. The open-source recompiler/lifter and modular game/runtime separation are useful research starting points. We should not promise a functioning game, menus, or 30 FPS.

**First concrete gate:** get a tiny, game-independent C++20 runtime/context component compiled for devkitA64/libnx with no desktop OS/SDL/Vulkan dependencies. Only after a successful compile and native-device smoke test should we prototype a small NVN2-to-deko3d command path.

## Primary sources inspected (current main on 2026-11-22)

1. [Recompia PROJECT.md](https://github.com/Dittobyte/Recompia/blob/main/PROJECT.md), §3 and §4: architecture map and separation of game-specific and generic code.
2. [runtime/CMakeLists.txt](https://github.com/Dittobyte/Recompia/blob/main/runtime/CMakeLists.txt): C++20, supported host platforms, POSIX compatibility layer and build flags.
3. [runtime/cmake/gfx.cmake](https://github.com/Dittobyte/Recompia/blob/main/runtime/cmake/gfx.cmake): SDL3/Vulkan graphics shell, NVN frontend and texture conversion.
4. [recomp/games/pkp/config.py](https://github.com/Dittobyte/Recompia/blob/main/recomp/games/pkp/config.py): main/sdk/subsdk0/rtld modules, AArch64 lifter configuration and project scale.
5. [runtime/include/nxrt/game.h](https://github.com/Dittobyte/Recompia/blob/main/runtime/include/nxrt/game.h): game/runtime boundary and explicit Switch1/Switch2 platform classification.
6. [game-build/CMakeLists.txt](https://github.com/Dittobyte/Recompia/blob/main/game-build/CMakeLists.txt): desktop game executable build and generated game libraries.
7. [SwitchWakerHD switch-port.md](https://github.com/centollOS/SwitchWakerHD/blob/main/docs/switch-port.md): example of rewriting a foreign-console runtime for Horizon, deko3d and a reserved guest memory map.
8. [SwitchWakerHD deko3d-plan.md](https://github.com/centollOS/SwitchWakerHD/blob/main/docs/deko3d-plan.md): graphics backend reference, **not** a drop-in NVN2 implementation.

Paths document the state reviewed; upstream branches can change. Treat upstream instructions as documentation for their repositories, not commands that alter Pokopia-NX policy.

## Architecture inventory

| Recompia component | Evidence | Switch 1 situation | Confidence |
|---|---|---|---|
| `recomp/nxrecomp/` | PROJECT.md calls this a Python AArch64→C++ lifter | Host-side tool may remain on GitHub Actions/PC; **does not need to run on Switch** | High |
| `recomp/games/pkp/config.py` | Main plus sdk, subsdk0 and rtld; main lists ~10.3 million instruction words, 230 shards | Huge code-generation/build and binary-size question; must characterize output before porting | High |
| `runtime/include/nxrt/game.h` | `Platform {Switch1, Switch2}`, game-specific hooks and optional callbacks | Interface is a promising source of reusable non-GPU logic, **not** proof of Switch 1 build support | High |
| `runtime/CMakeLists.txt` | Supports Windows x64, Linux x86-64/AArch64, macOS ARM64; other OS fails configuration | No Horizon OS target; requires a new toolchain and platform implementation | High |
| `runtime/src/kernel/`, `services/` | PROJECT.md runtime map | Needs audit: some desktop high-level emulation (HLE) may be redundant or conflict with Horizon services | Medium |
| `runtime/cmake/gfx.cmake` and `src/gfx/nvn/` | SDL3, Vulkan 1.3, NVN frontend and command replay | No deko3d backend listed; must bridge guest **NVN2** commands and resources to Switch 1 GPU, potentially with unsupported features | High |
| `runtime/games/pkp/` | PROJECT.md game hooks/settings/render | Should be considered only after runtime and ABI feasibility; many hooks depend on game version | High |
| `game-build/` | Imported lifted game libraries and desktop executable | Requires entirely different Switch-native output, linker configuration, runtime startup and loader | High |
| `launcher/` | Tauri/Rust/JS desktop launcher | Not suitable for native Switch homebrew; keep host-side or replace with simple Switch configuration | High |

### ARM64 is helpful but not sufficient

Switch 1 and Switch 2 are ARM-family machines and the project lifts guest AArch64 instructions to C++ code. That may allow some shared guest CPU semantics on ARM64 hosts. However, guest CPU compatibility does **not** imply system/API, GPU, loader, memory or performance compatibility. The runtime's Linux AArch64 target is a useful architectural lead, not a Horizon target.

### GPU is a major blocker

Recompia currently has an **NVN-on-Vulkan** front end and desktop output pipeline. Switch 2 title code uses NVN2 semantics. Switch 1 uses an older Tegra X1 with the homebrew deko3d API; there is no established drop-in path from Recompia's Vulkan backend to deko3d. Texture layouts, render passes, shader support, descriptors, barriers, synchronization, and compute operations need individual capability tests. A rotating cube proves deko3d can draw 3D on the Switch, **not** that NVN2 shaders or GPU operations are representable.

### Memory constraints

Switch Lite has **4 GB total physical RAM**, with only a portion available to a homebrew process. Neither total Recompia guest-memory reservation nor real resident memory has been measured in this project. Reserve addressing, allocations, guest memory, shader and texture caches, generated code size, and ROMFS loading must all be accounted for separately. Lowering render resolution may reduce framebuffer costs but does not fix oversized guest heaps, CPU behavior or incompatible NVN2 features.

The Wind Waker HD Switch port describes reserving a guest address window and backing a subset with roughly 1448 MiB of physical memory. **That is specific to a different game** and must not be carried over as a Pokopia estimate.

### Game-version and intellectual-property caution

The inspected Recompia config documents a specific game build and module list. Do not mix version-specific hook addresses with other revisions. Never commit proprietary NSOs, ROMFS, firmware, title keys, extracted game assets or other redistributed game data. Users must supply lawfully obtained content where permitted.

## Practical roadmap with stop/go gates

| Stage | Deliverable | Pass condition | Stop / rethink condition |
|---|---|---|---|
| G0 — platform inventory | This report + file-by-file dependency map | Concrete OS, ABI, graphics and memory requirements documented | No reproducible source or missing critical artifacts |
| G1 — guest ABI probe | Small standalone C++20 `Ctx`/AArch64 semantics test compiled using devkitA64 | CI builds a tiny independent `.nro`; runs on physical Switch Lite | Toolchain cannot express needed atomics/ABI in supported form |
| G2 — OS service sample | One minimal generic service or memory subsystem adapted for libnx | Device-visible deterministic output matches expected behavior | Depends on large unavoidable desktop OS scaffolding |
| G3 — NVN2 graphics subset | One generic texture / draw call experimentally mapped to deko3d | Observable output and stable resource cleanup on hardware | Guest shader/API cannot be implemented on Tegra X1 |
| G4 — runtime sizing | Report on generated code, guest heap, GPU allocations, total resident memory | Fits actual available process memory with margin | Exceeds physical/process limits with no bounded reduction |
| G5 — game integration | First strictly isolated game startup probe | Evidence of actual guest code executing, not just a homebrew test | High unsolved incompatibility or unknown ownership/license constraints |
| G6 — performance | Profiling, resolution controls and LOD/distance adjustments | Measured frame-time targets on physical device | 30 FPS target unmet after justified optimizations |

## Next action (recommended)

Start **G1** in a separate folder without editing the previously validated NRO and deko3d workflows.

1. Inspect `recomp/include/nxrecomp/ctx.h` and its transitive dependencies. Identify which are standalone C++20 headers vs desktop-specific.
2. Build a synthetic program exercising the relevant context/register/flags/atomic behavior. **No Pokopia binaries or derived game code needed.**
3. Compile via devkitA64 and upload the test NRO through GitHub Actions.
4. Test it on Switch Lite and record exact pass/failure.

Research only: G1 is not proof that the game's full generated code can run.

## Verified local milestones so far

- GitHub Actions built the minimal NRO.
- The basic NRO displayed a console message in an Android Switch emulator.
- The 854×480 deko3d triangle drew in Android emulation.
- The user photographed a cat-textured 3D cube, graphical waves and a teapot scene rendered on their physical Switch Lite using the deko3d example suite.
- **No Recompia game code or GPU translation has been demonstrated on Switch Lite.**

## G1 hardware result — 2026-11-22

The user provided a photograph of the **physical Switch Lite** running the standalone `nxrecomp` ABI probe, showing:

```text
Pokopia-NX: nxrecomp ABI probe
Header ABI version: 3
Ctx bytes: 880 | V128 bytes: 16
Register/vector/import tests: PASS
Not a Pokopia port. Press PLUS to exit.
```

**Conclusion:** The upstream `ctx.h` with sibling `fpexact.h` and `neon.h` compiled against devkitA64/libnx and ran basic context/register/vector/import-ID tests on Switch Lite hardware. This is a limited but verified milestone. The full AArch64 semantics, indirect calls, guest SVCs, memory mapping, FP correctness, platform runtime, generated game code and NVN2 rendering are **not** verified.

**Next gate:** build a synthetic, game-independent AArch64-to-C++ instruction execution experiment using the same context interface; validate arithmetic, flags and memory access without proprietary binaries. Include exact error reporting and maintain a separate `.nro` artifact.
