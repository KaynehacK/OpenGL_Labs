#version 330 core
in vec2 uv;
out vec4 color;

void main() {
    float value;
    vec2 center = vec2(0.5, 0.5);
    float radius = length(uv * 0.5 + 0.5 - center);
    value = sin(radius * 120.0);

    if (value - 0.2 > 0.0)
        color = vec4(1.0);
    else
        color = vec4( 0.0, 0.0, 0.0, 1.0);
}