# Switch Lite graphics benchmark

[GitHub Actions builds the NRO](https://github.com/synxeren2/pokopia-nx/actions/workflows/graphics-benchmark.yml). Extract the artifact and put `nx-graphics-benchmark.nro` in the SD card's `switch` directory. Launch from hbmenu. The tests run automatically for around half a minute at 30 FPS. Plus skips to the results, or exits from the results screen.

| Internal framebuffer | Rows drawn | Triangles per tile |
| --- | ---: | ---: |
| 854 × 480 | 12 | 1 |
| 640 × 360 (75% scale) | 12 | 1 |
| 854 × 480 | 12 | 2 |
| 854 × 480 | 36 | 2 |

The framebuffer is presented at the device's display size. Each row represents a distance band in the synthetic scene. Rows are drawn from the center outward. The triangle count models a basic geometry LOD. The cap uses one presentation per two refreshes on a 60 Hz display.

Each mode discards its first 60 frames. Results report average FPS, 95th-percentile interval in milliseconds, and the count of intervals above 34 ms from 180 measurements. Intervals are sampled after the present call and include swapchain pacing and queue pressure. The scene uses original procedural geometry. These measurements apply to this workload on the device used for the test.

The build adapts the pinned [switchbrew deko_basic sample](https://github.com/switchbrew/switch-examples/tree/669786898205b7beb25ff1731e72982e6d0397d3/graphics/deko3d/deko_basic).
