#version 430 core

layout (location = 0) in vec2 a_position;

uniform vec2 u_position;
uniform vec2 u_size;
uniform vec2 u_anchor;
uniform vec2 u_screenSize;

uniform vec4 u_color;

out vec4 v_color;

void main() {
    vec2 topLeft = u_position - u_size * u_anchor;
    vec2 pixelPosition = topLeft + a_position * u_size;

    vec2 ndc = pixelPosition / u_screenSize * 2.0 - 1.0;
    ndc.y = -ndc.y;

    gl_Position = vec4(ndc, 0.0, 1.0);

    v_color = u_color;
}