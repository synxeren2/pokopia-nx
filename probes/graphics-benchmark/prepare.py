"""Adapt the pinned switchbrew deko_basic sample in the CI build directory."""
from pathlib import Path
import shutil
import sys


project = Path(sys.argv[1])
probe = Path(__file__).resolve().parent
source = project / "source" / "main.c"
code = source.read_text(encoding="utf-8")

replacements = {
    "static void graphicsInitialize(void)": (
        "static void graphicsInitialize(unsigned width, unsigned height, "
        "unsigned rows, unsigned vertices)"
    ),
    "imageLayoutMaker.dimensions[0] = FB_WIDTH;": "imageLayoutMaker.dimensions[0] = width;",
    "imageLayoutMaker.dimensions[1] = FB_HEIGHT;": "imageLayoutMaker.dimensions[1] = height;",
    "g_swapchain = dkSwapchainCreate(&swapchainMaker);": (
        "g_swapchain = dkSwapchainCreate(&swapchainMaker);\n"
        "    dkSwapchainSetSwapInterval(g_swapchain, 2);"
    ),
    "(float)FB_WIDTH, (float)FB_HEIGHT": "(float)width, (float)height",
    "{ 0, 0, FB_WIDTH, FB_HEIGHT }": "{ 0, 0, width, height }",
    "DkPrimitive_Triangles, 3, 1, 0, 0": "DkPrimitive_Triangles, vertices, 48 * rows, 0, 0",
}

for original, replacement in replacements.items():
    if code.count(original) != 1:
        raise SystemExit(f"Unexpected upstream source: {original!r}")
    code = code.replace(original, replacement)

marker = "// Main entrypoint\n"
if code.count(marker) != 1:
    raise SystemExit("Unexpected upstream main entrypoint")
source.write_text(code.split(marker)[0] + '#include "benchmark.inc"\n', encoding="utf-8")
shutil.copyfile(probe / "benchmark.inc", source.parent / "benchmark.inc")
shutil.copyfile(probe / "tiles_vsh.glsl", source.parent / "triangle_vsh.glsl")
