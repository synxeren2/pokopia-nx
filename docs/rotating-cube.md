# Third milestone: rotating 3D cube

**Status:** Build workflow submitted; hardware and emulator runtime results pending.

## Purpose

Our 480p triangle test successfully displayed on an Android Switch emulator (~59.9 FPS reported). The next useful milestone is a real 3D test with:
- A vertex buffer containing a cube
- Model-view and perspective projection matrices
- A rotating model
- Depth-buffer allocation, depth testing and correct face visibility
- Uniform-buffer updates during rendering

Instead of inventing an untested implementation, the [official switchbrew deko3d examples](https://github.com/switchbrew/switch-examples/tree/master/graphics/deko3d/deko_examples) contain **Example 03: Cube**, which specifically covers these features.

## Build and test

1. Open [Build rotating 3D cube test](../.github/workflows/cube-test.yml) under GitHub Actions and run it on main.
2. Download the `pokopia-nx-3d-cube-test` workflow artifact ZIP.
3. Extract `deko_examples.nro` and load it in the Android Switch emulator.
4. In the deko3d examples menu, choose **03: Cube** (D-pad then A).
5. Verify that the cube rotates and its back faces are properly hidden. Take a screenshot and note errors, emulator version and reported FPS.
6. For an actual performance baseline, retest on a physical Switch Lite.

**Caveat:** This stage uses the example's existing display-resolution behavior. It does not yet implement selectable 360p/480p/720p render scaling, GPU benchmarking, or a Pokopia rendering backend. The menu includes other examples but those are not prerequisites for this test.

## Source provenance

The CI workflow fetches and builds upstream `switchbrew/switch-examples` code instead of checking third-party code into this repository. Refer to its upstream license and authors. Do not confuse upstream example code with an original Pokopia-NX implementation.

## Exit criteria

- [ ] CI builds successfully and uploads an NRO.
- [ ] Emulator renders a continuously rotating cube.
- [ ] Depth testing appears correct.
- [ ] Real Switch Lite test performed.
- [ ] After verification, add resolution controls and per-frame timing measurements.
