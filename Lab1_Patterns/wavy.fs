#version 330 core
in vec2 uv;
out vec4 color;

void main() {
    if (sin(uv.y * 60 + sin(uv.x * 10.0) * 5) - 0.2 > 0.0)
        color = vec4(1.0);
    else
        color = vec4( 0.0, 0.0, 0.0, 1.0);
}