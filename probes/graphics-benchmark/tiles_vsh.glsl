#version 460

// The first triangle is the low-detail mesh. The second completes the tile.
const vec2 corners[6] = vec2[](
    vec2(0, 0), vec2(1, 0), vec2(0, 1),
    vec2(1, 0), vec2(1, 1), vec2(0, 1)
);

layout (location = 0) out vec4 outColor;

void main()
{
    int column = gl_InstanceID % 48;
    int row = gl_InstanceID / 48;
    // Add rows outward from the center as the draw distance increases.
    int y = (row % 2 == 0) ? 18 + row / 2 : 17 - row / 2;
    vec2 grid = vec2(column, y) + vec2(0.05) + corners[gl_VertexID] * 0.9;
    gl_Position = vec4(grid / vec2(48.0, 36.0) * 2.0 - 1.0, 0.0, 1.0);
    outColor = vec4(0.2 + float(column % 8) * 0.07, 0.6, 0.3 + float(y % 6) * 0.08, 1.0);
}
