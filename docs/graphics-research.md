# Graphics feasibility research — Pokopia-NX

Status: **research / unverified GPU probe** (2026-11-22).

## What is verified

- The repo's basic `libnx` NRO built successfully in GitHub Actions (run #4).
- The user launched that console-output NRO in an Android Switch emulator and observed the expected text at ~60 FPS.
- This is **not** evidence that Pokopia's engine, graphics, or performance work on Switch 1.

## GPU approach

The original Switch uses Nvidia Tegra X1. The homebrew-native low-level graphics API is **deko3d**. Shader compilation typically uses **UAM** to produce DKSH binaries at build time.

Reference documentation:
- https://github.com/devkitPro/deko3d
- https://github.com/devkitPro/deko3d/blob/master/Primer.md
- https://github.com/devkitPro/uam
- https://github.com/switchbrew/switch-examples/tree/master/graphics/deko3d/deko_basic

A separate CI workflow (`.github/workflows/gpu-probe.yml`) fetches the official `deko_basic` sample, changes its framebuffer from 1280x720 to 854x480, builds the triangle demo, and uploads its NRO. This intentionally does not overwrite the working console prototype.

**Attribution:** The GPU example's source comes from `switchbrew/switch-examples`; we build upstream source in CI instead of copying it into this repository. Check upstream licensing before redistributing a modified source tree.

## Important caveats

- 854x480 is an initial **render-target experiment**, not dynamic resolution or 3D render scaling.
- Drawing one triangle is a graphics pipeline smoke test, not a Pokopia port or benchmark.
- A successful CI build does **not** verify that the NRO renders correctly on Switch Lite.
- An Android emulator result does **not** establish real-hardware GPU compatibility.
- Genuine 3D work will require vertex/index buffers, transform matrices, depth testing, and profiling; the triangle test only exercises part of the pipeline.
- Recompia currently targets desktop operating systems. Its graphics/runtime cannot be assumed to work with deko3d unchanged.

## Next experiments (after hardware verification)

1. Verify the triangle demo starts, renders and exits with PLUS.
2. Record environment: real Switch Lite firmware/CFW, or emulator name and build.
3. Implement a rotating, depth-tested cube with clear separation of camera and model transforms.
4. Measure frame time / memory for 720p, 540p, 480p, 360p GPU output. Separate render resolution from display presentation.
5. Inspect Recompia's runtime dependencies and map required graphics operations to deko3d.
6. Identify blockers and stop if the required runtime exceeds hardware capabilities.

## Relevant porting references

- https://github.com/Dittobyte/Recompia
- https://github.com/Dittobyte/Recompia/blob/main/PROJECT.md
- https://github.com/centollOS/SwitchWakerHD/blob/main/docs/switch-port.md

Do not add copyrighted game data, keys, firmware, or proprietary assets to the repo.
